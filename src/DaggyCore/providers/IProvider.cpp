/*
MIT License

Copyright (c) 2021 Mikhail Milovidov

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
#include "Precompiled.hpp"
#include "daggycore_export.h"
#include "IProvider.hpp"

daggy::providers::IProvider::IProvider(sources::Commands commands,
                                       QObject* parent)
    : QObject(parent)
    , commands_(std::move(commands))
    , state_(DaggyProviderNotStarted)
{

}

daggy::providers::IProvider::~IProvider()
{

}

const daggy::sources::Commands& daggy::providers::IProvider::commands() const noexcept
{
    return commands_;
}

bool daggy::providers::IProvider::commandSupported(const QString& id) const noexcept
{
    return commands_.find(id) != commands_.cend();
}

const daggy::sources::commands::Properties& daggy::providers::IProvider::getCommandProperties(const QString& id) const noexcept
{
    auto result = commands_.find(id);
    assert(result != commands_.cend());
    return result.value();
}

DaggyProviderStates daggy::providers::IProvider::state() const noexcept
{
    return state_;
}

int daggy::providers::IProvider::restartCommandsCount() const noexcept
{
    int result = 0;
    for (const auto& command : commands_)
        if (command.restart)
            result++;
    return result;
}

void daggy::providers::IProvider::setState(DaggyProviderStates state)
{
    if (state_ == state)
        return;

    state_ = state;
    emit stateChanged(state_);
}

daggy::sources::commands::streams::Meta daggy::providers::IProvider::metaStream(const QString& id, DaggyStreamTypes type, bool reset)
{
    const auto& properties = getCommandProperties(id);
    QString meta_stream_id = QString("%1%2").arg(id, type);
    if (reset)
        streams_meta_.remove(meta_stream_id);
    else if (!streams_meta_.contains(meta_stream_id))
    {
        const auto now = std::chrono::steady_clock::now();
        streams_meta_.insert(meta_stream_id, {now,  properties.extension, type, 0, now});
    }
    else
    {
        auto& meta = streams_meta_[meta_stream_id];
        meta.seq_num++;
        meta.time = std::chrono::steady_clock::now();
    }
    return streams_meta_[meta_stream_id];
}
