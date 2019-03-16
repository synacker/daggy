/*
Copyright 2017-2018 Mikhail Milovidov <milovidovmikhail@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "Precompiled.h"
#include "IRemoteAgregator.h"

using namespace daggycore;

IRemoteAgregator::IRemoteAgregator(QObject* pParent)
    : QObject(pParent)
    , state_(State::Stopped)
    , running_remote_command_count_(0)
{
}

bool IRemoteAgregator::isExistsRunningRemoteCommands() const
{
    return runingRemoteCommandsCount() > 0;
}

size_t IRemoteAgregator::runingRemoteCommandsCount() const
{
    return running_remote_command_count_;
}

void IRemoteAgregator::start()
{
    if (state_ == State::Stopped) {
        setState(State::Run);
        startAgregator();
    }
}

void IRemoteAgregator::stop()
{
    if (state_ == State::Run) {
        setState(State::Stopping);
        stopAgregator();
    }
}

void IRemoteAgregator::setState(const IRemoteAgregator::State state)
{
    if (state_ != state) {
        state_ = state;
        emit stateChanged(state);
    }
}

IRemoteAgregator::State IRemoteAgregator::state() const
{
    return state_;
}

void IRemoteAgregator::setStopped()
{
    setState(State::Stopped);
}
