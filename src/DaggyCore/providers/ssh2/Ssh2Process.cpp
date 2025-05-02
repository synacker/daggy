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
#include "../../Precompiled.hpp"
#include "Ssh2Process.hpp"

#include "Ssh2Client.hpp"
#include "Ssh2Debug.hpp"

using namespace qtssh2;

Ssh2Process::Ssh2Process(const QString& command, Ssh2Client* ssh2_client)
    : Ssh2Channel(ssh2_client)
    , command_(command)
    , ssh2_process_state_(NotStarted)
{
    connect(this, &Ssh2Channel::channelStateChanged, this, &Ssh2Process::onSsh2ChannelStateChanged);
}

Ssh2Process::ProcessStates Ssh2Process::processState() const
{
    return ssh2_process_state_;
}

bool Ssh2Process::setSsh2ProcessState(ProcessStates ssh2_process_state)
{
    if (ssh2_process_state_ == ssh2_process_state)
        return false;

    ssh2_process_state_ = ssh2_process_state;
    emit processStateChanged(ssh2_process_state_);
    return true;
}

void Ssh2Process::onSsh2ChannelStateChanged(ChannelStates state)
{
    std::error_code error_code = ssh2_success;
    switch (state) {
    case ChannelStates::Opened:
        setSsh2ProcessState(Starting);
        if (!checkSsh2Error(execSsh2Process()))
        {
            setSsh2ProcessState(FailedToStart);
        }
        break;
    case ChannelStates::Closing:
        if (ssh2_process_state_ != FailedToStart)
            setSsh2ProcessState(Finishing);
        break;
    case ChannelStates::Closed:
        if (ssh2_process_state_ != FailedToStart)
            setSsh2ProcessState(Finished);
        break;
    case ChannelStates::FailedToOpen:
        setSsh2ProcessState(FailedToStart);
        break;
    default:;
    }
}

void Ssh2Process::checkIncomingData()
{
    switch (ssh2_process_state_) {
    case NotStarted:
        if (isOpen())
            Ssh2Channel::checkIncomingData();
        break;
    case Starting:
        execSsh2Process();
        break;
    case Started:
    case Finishing:
        Ssh2Channel::checkIncomingData();
        break;
    default:;
    }
}

std::error_code Ssh2Process::execSsh2Process()
{
    std::error_code error_code = ssh2_success;
    const int ssh2_method_result = libssh2_channel_exec(ssh2Channel(), qPrintable(command_));
    switch (ssh2_method_result) {
    case LIBSSH2_ERROR_EAGAIN:
        error_code = Ssh2Error::TryAgain;
        break;
    case 0:
        setSsh2ProcessState(Started);
        break;
    default: {
        setSsh2ProcessState(FailedToStart);
        debugSsh2Error(ssh2_method_result);
        error_code = Ssh2Error::ProcessFailedToStart;
        close();
    }
    }
    return error_code;
}
