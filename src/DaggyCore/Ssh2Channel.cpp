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
#include "Precompiled.h"
#include "Ssh2Channel.h"

#include "Ssh2Client.h"
#include "Ssh2Debug.h"

using namespace daggyssh2;

Ssh2Channel::Ssh2Channel(Ssh2Client* ssh2_client)
    : QIODevice(ssh2_client)
    , ssh2_channel_state_(ChannelStates::NotOpen)
    , ssh2_channel_(nullptr)
    , exit_status_(-1)
    , exit_signal_("none")
    , last_error_(ssh2_success)
{
}

Ssh2Channel::~Ssh2Channel()
{
    destroyChannel();
}

qint64 Ssh2Channel::writeData(const char* data, qint64 len)
{
    if (ssh2_channel_ == nullptr)
        return -1;
    ssize_t result = libssh2_channel_write_ex(ssh2_channel_, 0, data, len);
    if (result < 0 && result != LIBSSH2_ERROR_EAGAIN) {
        switch (result) {
        case LIBSSH2_ERROR_CHANNEL_CLOSED:
            destroyChannel();
            break;
        default:
            setLastError(Ssh2Error::ChannelWriteError);
        }
        result = -1;
    }
    return result;
}

qint64 Ssh2Channel::readData(char* data, qint64 maxlen)
{
    if (ssh2_channel_ == nullptr)
        return -1;

    ssize_t result = libssh2_channel_read_ex(ssh2_channel_, currentReadChannel(), data, maxlen);
    if (result < 0 && result != LIBSSH2_ERROR_EAGAIN) {
        switch (result) {
        case LIBSSH2_ERROR_CHANNEL_CLOSED:
            destroyChannel();
            break;
        default:
            setLastError(Ssh2Error::ChannelReadError);
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
    if (ssh2_channel_)
        return true;
    if (ssh2Client()->sessionState() != Ssh2Client::Established)
        return false;

    std::error_code error_code = openSession();
    setLastError(error_code);
    return checkSsh2Error(error_code);
}

void Ssh2Channel::close()
{
    if (ssh2_channel_ == nullptr)
        return;
    if (ssh2_channel_state_ == Opened) {
        emit aboutToClose();
        //libssh2_channel_send_eof(ssh2_channel_);
        std::error_code error_code = closeSession();
        setLastError(error_code);
    } else
        destroyChannel();
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

void Ssh2Channel::checkIncomingData()
{
    std::error_code error_code = ssh2_success;
    switch (ssh2_channel_state_) {
    case ChannelStates::Opening:
        error_code = openSession();
        break;
    case ChannelStates::Opened:
        checkChannelData();
        if (libssh2_channel_eof(ssh2_channel_) == 1) {
            close();
        }
        break;
    case ChannelStates::Closing:
        checkChannelData();
        error_code = closeSession();
        break;
    default:;
    }

    setLastError(error_code);
}

int Ssh2Channel::exitStatus() const
{
    return exit_status_;
}

Ssh2Channel::ChannelStates Ssh2Channel::channelState() const
{
    return ssh2_channel_state_;
}

void Ssh2Channel::setSsh2ChannelState(const ChannelStates& state) {
    if (ssh2_channel_state_ != state) {
        ssh2_channel_state_ = state;
        emit channelStateChanged(ssh2_channel_state_);
    }
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
        setSsh2ChannelState(Opening);
        error_code = Ssh2Error::TryAgain;
        break;
    case 0:
        QIODevice::open(QIODevice::ReadWrite | QIODevice::Unbuffered);
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
    const int ssh2_method_result = libssh2_channel_send_eof(ssh2_channel_);
    switch (ssh2_method_result) {
    case LIBSSH2_ERROR_EAGAIN:
        setSsh2ChannelState(ChannelStates::Closing);
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
        destroyChannel();
    }
        break;
    default: {
        debugSsh2Error(ssh2_method_result);
        destroyChannel();
    }
    }
    return error_code;
}

void Ssh2Channel::destroyChannel()
{
    if (ssh2_channel_ == nullptr)
        return;
    setOpenMode(QIODevice::NotOpen);
    if (ssh2_channel_state_ != ChannelStates::FailedToOpen)
        setSsh2ChannelState(ChannelStates::Closed);
    libssh2_channel_free(ssh2_channel_);
    ssh2_channel_ = nullptr;
}

void Ssh2Channel::checkChannelData(const Ssh2Channel::ChannelStream& stream_id)
{
    switch (stream_id) {
    case Out:
        setCurrentReadChannel(0);
        break;
    case Err:
        setCurrentReadChannel(1);
        break;
    }
    const QByteArray data = readAll();
    if (data.size())
        emit newChannelData(data, stream_id);
}

QString Ssh2Channel::exitSignal() const
{
    return exit_signal_;
}

std::error_code Ssh2Channel::setLastError(const std::error_code& error_code)
{
    if (last_error_ != error_code && error_code != Ssh2Error::TryAgain) {
        last_error_ = error_code;
        emit ssh2Error(last_error_);
    }
    return error_code;
}
