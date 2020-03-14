/*
Copyright 2017-2020 Milovidov Mikhail

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "Precompiled.h"
#include "IDataProvider.h"

#include "Common.h"

using namespace daggycore;

IDataProvider::IDataProvider(Commands commands,
                             QObject *parent)
    : QObject(parent)
    , commands_(std::move(commands))
    , state_(State::NotStarted)
{

}

const Commands& IDataProvider::commands() const
{
    return commands_;
}

Command IDataProvider::getCommand(const QString& id) const
{
    return commands_.value(id);
}

IDataProvider::State IDataProvider::state() const
{
    return state_;
}

int IDataProvider::restartCommandsCount() const
{
    int result = 0;
    for (const auto& command : commands_)
        if (command.restart)
            result++;
    return result;
}

void IDataProvider::setState(IDataProvider::State providerState)
{
    if (state_ == providerState)
        return;

    state_ = providerState;
    emit stateChanged(state_);
}
