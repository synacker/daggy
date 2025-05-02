/*
MIT License

Copyright (c) 2020 Mikhail Milovidov

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#include "Ssh2Channel.hpp"

#include "Ssh2Client.hpp"
#include "Ssh2Debug.hpp"

using namespace qtssh2;

Ssh2Channel::Ssh2Channel(Ssh2Client* ssh2_client)
    : QIODevice(ssh2_client)
    , ssh2_channel_state_(ChannelStates::NotOpen)
    , ssh2_channel_(nullptr)
    , exit_status_(-1)
    , exit_signal_("none")
{
    connect(ssh2_client, &QTcpSocket::readyRead, this, &Ssh2Channel::onSshReadyRead);
    connect(this, &QIODevice::aboutToClose, this, &Ssh2Channel::onAboutToClose);
}

Ssh2Channel::~Ssh2Channel()
{
}

qint64 Ssh2Channel::writeData(const char* data, qint64 len)
{
    if (ssh2_channel_ == nullptr)
        return -1;
    ssize_t result = libssh2_channel_write_ex(ssh2_channel_, 0, data, len);
    if (result < 0 && result != LIBSSH2_ERROR_EAGAIN) {
        switch (result) {
        case LIBSSH2_ERROR_CHANNEL_CLOSED:
            setSsh2ChannelState(ChannelStates::Closed);
            break;
        }
        result = -1;
    }
    return result;
}

qint64 Ssh2Channel::readData(char* data, qint64 maxlen)
{
    if (ssh2_channel_ == nullptr)
        return -1;

    ssize_t result = libssh2_channel_read_ex(ssh2_channel_, 0, data, maxlen);
    if (result < 0) {
        switch (result) {
        case LIBSSH2_ERROR_CHANNEL_CLOSED:
            setSsh2ChannelState(ChannelStates::Closed);
            break;
        }

        result = -1;
    }

    return result;
}

bool Ssh2Channel::isSequential() const
{
    return true;
}

bool Ssh2Channel::open(QIODevice::OpenMode)
{
    if (isOpen())
        return true;

    if (ssh2_channel_)
        return true;
    if (ssh2Client()->sessionState() != Ssh2Client::Established)
        return false;

    if (setSsh2ChannelState(Opening) && ssh2_channel_state_ == Opening) {
        QIODevice::open(QIODevice::ReadWrite | QIODevice::Unbuffered);
    }

    return isOpen();
}

Ssh2Client* Ssh2Channel::ssh2Client() const
{
    return qobject_cast<Ssh2Client*>(parent());
}

LIBSSH2_CHANNEL* Ssh2Channel::ssh2Channel() const
{
    return ssh2_channel_;
}

void Ssh2Channel::checkChannelData()
{
    checkChannelData(Out);
    checkChannelData(Err);
}

void Ssh2Channel::readErrStream()
{
    QBuffer buffer;
    constexpr const size_t buffer_size = 1024;
    char data[buffer_size];
    ssize_t result = buffer_size;
    while (result > 0) {
        result = libssh2_channel_read_ex(ssh2_channel_, SSH_EXTENDED_DATA_STDERR, data, buffer_size);
        if (result > 0)
            buffer.write(data, result);
    }
    if (buffer.size() > 0)
        emit newChannelData(buffer.data(), ChannelStream::Err);
}

void Ssh2Channel::checkIncomingData()
{
    std::error_code error_code = ssh2_success;
    switch (ssh2_channel_state_) {
    case ChannelStates::Opening:
        openSession();
        break;
    case ChannelStates::Opened:
        checkChannelData();
        if (libssh2_channel_eof(ssh2_channel_) == 1) {
            close();
        }
        break;
    case ChannelStates::Closing:
        closeSession();
        break;
    default:;
    }
}

int Ssh2Channel::exitStatus() const
{
    return exit_status_;
}

Ssh2Channel::ChannelStates Ssh2Channel::channelState() const
{
    return ssh2_channel_state_;
}

bool Ssh2Channel::setSsh2ChannelState(ChannelStates state) {
    if (ssh2_channel_state_ == state)
        return false;

    switch (state) {
    case Opening:
        if (!checkSsh2Error(openSession())) {
            state = FailedToOpen;
        }
        break;
    case Closing:
        if (!checkSsh2Error(closeSession())) {
            state = Closed;
        }
        break;
    default:;
    }

    ssh2_channel_state_ = state;
    if (ssh2_channel_state_ == Closed || ssh2_channel_state_ == FailedToOpen)
        destroyChannel();
    emit channelStateChanged(ssh2_channel_state_);

    return true;
}

std::error_code Ssh2Channel::openSession()
{
    std::error_code error_code = ssh2_success;
    int ssh2_method_result = 0;

    LIBSSH2_SESSION* const ssh2_session = ssh2Client()->ssh2Session();
    ssh2_channel_ = libssh2_channel_open_session(ssh2_session);
    if (ssh2_channel_ == nullptr)
        ssh2_method_result = libssh2_session_last_error(ssh2Client()->ssh2Session(),
                                                        nullptr,
                                                        nullptr,
                                                        0);
    switch (ssh2_method_result) {
    case LIBSSH2_ERROR_EAGAIN:
        error_code = Ssh2Error::TryAgain;
        break;
    case 0:
        setSsh2ChannelState(Opened);
        error_code = ssh2_success;
        break;
    default: {
        debugSsh2Error(ssh2_method_result);
        error_code = Ssh2Error::FailedToOpenChannel;
        setSsh2ChannelState(FailedToOpen);
    }
    }
    return error_code;
}

std::error_code Ssh2Channel::closeSession()
{
    std::error_code error_code = ssh2_success;
    libssh2_channel_flush_ex(ssh2_channel_, 0);
    libssh2_channel_flush_ex(ssh2_channel_, 1);
    const int ssh2_method_result = libssh2_channel_close(ssh2_channel_);
    switch (ssh2_method_result) {
    case LIBSSH2_ERROR_EAGAIN:
        error_code = Ssh2Error::TryAgain;
        break;
    case 0: {
        exit_status_ = libssh2_channel_get_exit_status(ssh2_channel_);
        char* exit_signal = (char*)"none";
        const int result = libssh2_channel_get_exit_signal(ssh2_channel_,
                                                           &exit_signal,
                                                           nullptr,
                                                           nullptr,
                                                           nullptr,
                                                           nullptr,
                                                           nullptr);
        if (result == 0)
            exit_signal_ = QString(exit_signal);
        setSsh2ChannelState(ChannelStates::Closed);
    }
    break;
    default: {
        debugSsh2Error(ssh2_method_result);
        setSsh2ChannelState(ChannelStates::Closed);
    }
    }
    return error_code;
}

void Ssh2Channel::destroyChannel()
{
    if (ssh2_channel_ == nullptr)
        return;

    libssh2_channel_free(ssh2_channel_);
    ssh2_channel_ = nullptr;
}

void Ssh2Channel::checkChannelData(const Ssh2Channel::ChannelStream& stream_id)
{
    switch (stream_id) {
    case Out:
    {
        const QByteArray data = readAll();
        if (data.size() > 0)
            emit newChannelData(data, stream_id);
    }
        break;
    case Err:
        readErrStream();
        break;
    }
}

void Ssh2Channel::onAboutToClose()
{
    if (ssh2_channel_ == nullptr)
        setSsh2ChannelState(ChannelStates::Closed);
    setSsh2ChannelState(ChannelStates::Closing);
}

void Ssh2Channel::onSshReadyRead()
{
    switch (ssh2Client()->sessionState()) {
    case Ssh2Client::Closing:
    case Ssh2Client::Established:
        checkIncomingData();
        break;
    default:;
    }
}
