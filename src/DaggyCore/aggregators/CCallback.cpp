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
#include "../Precompiled.hpp"
#include "CCallback.hpp"

namespace  {
DaggyError convert(const std::error_code& error)
{
    return {error.value(), error.category().name()};
}
}

daggy::aggregators::CCallback::CCallback(libdaggy_on_daggy_state_changed on_daggy_state_changed,
                                         libdaggy_on_provider_state_changed on_provider_state_changed,
                                         libdaggy_on_provider_error on_provider_error,
                                         libdaggy_on_command_state_changed on_command_state_changed,
                                         libdaggy_on_command_stream on_command_stream, libdaggy_on_command_error on_command_error,
                                         QObject* parent)
    : IAggregator(parent)
    , on_daggy_state_changed_(on_daggy_state_changed)
    , on_provider_state_changed_(on_provider_state_changed)
    , on_provider_error_(on_provider_error)
    , on_command_state_changed_(on_command_state_changed)
    , on_command_stream_(on_command_stream)
    , on_command_error_(on_command_error)
{

}

bool daggy::aggregators::CCallback::isReady() const
{
    return true;
}

void daggy::aggregators::CCallback::onDataProviderStateChanged(QString provider_id, DaggyProviderStates state)
{
    if (on_provider_state_changed_)
    {
        auto core = sender();
        on_provider_state_changed_(core, qPrintable(provider_id), state);
    }
}

void daggy::aggregators::CCallback::onDataProviderError(QString provider_id, std::error_code error_code)
{
    if (on_provider_error_)
    {
        auto core = sender();
        on_provider_error_(core, qPrintable(provider_id), convert(error_code));
    }
}

void daggy::aggregators::CCallback::onCommandStateChanged(QString provider_id, QString command_id, DaggyCommandStates state, int exit_code)
{
    if (on_command_state_changed_)
    {
        auto core = sender();
        on_command_state_changed_(core, qPrintable(provider_id), qPrintable(command_id), state, exit_code);
    }
}

void daggy::aggregators::CCallback::onCommandError(QString provider_id, QString command_id, std::error_code error_code)
{
    if (on_command_error_)
    {
        auto core = sender();
        on_command_error_(core, qPrintable(provider_id), qPrintable(command_id), convert(error_code));
    }
}

void daggy::aggregators::CCallback::onCommandStream(QString provider_id, QString command_id, sources::commands::Stream stream)
{
    if (on_command_stream_)
    {
        auto core = sender();
        on_command_stream_(core, qPrintable(provider_id), qPrintable(command_id), {qPrintable(stream.meta.session),
                                                                                   std::chrono::system_clock::to_time_t(stream.meta.start_time),
                                                                                   qPrintable(stream.meta.extension),
                                                                                   stream.meta.type,
                                                                                   stream.meta.seq_num,
                                                                                   std::chrono::system_clock::to_time_t(stream.meta.time),
                                                                                   reinterpret_cast<const std::uint8_t*>(stream.part.data()),
                                                                                   stream.part.size()});
    }
}

void daggy::aggregators::CCallback::onDaggyStateChanged(DaggyStates state)
{
    if (on_daggy_state_changed_)
    {
        auto core = sender();
        on_daggy_state_changed_(core, state);
    }
}
