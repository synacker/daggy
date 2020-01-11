/*
Copyright 2017-2020 Milovidov Mikhail

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "Precompiled.h"
#include "CFileDataAggregator.h"

CFileDataAggregator::CFileDataAggregator()
{

}

void CFileDataAggregator::onDataProviderStateChanged(const QString provider_id,
                                                     const int state)
{
    qDebug() << QString("Provider state change: %1 in %2 state")
                .arg(provider_id)
                .arg(state);
}

void CFileDataAggregator::onDataProviderError(const QString provider_id,
                                              const std::error_code error_code)
{
    qDebug() << QString("Provider error: %1 - %2")
                .arg(provider_id)
                .arg(error_code.message().c_str());
}

void CFileDataAggregator::onCommandStateChanged(const QString provider_id,
                                                const QString command_id,
                                                const daggycore::Command::State state,
                                                const int exit_code)
{
    qDebug() << QString("Command state change: %1_%2 in %3 state with exit code %4")
                .arg(provider_id)
                .arg(command_id)
                .arg(state)
                .arg(exit_code);
}

void CFileDataAggregator::onCommandStream(const QString provider_id,
                                          const QString command_id,
                                          const daggycore::Command::Stream stream)
{
    qDebug() << QString("\nCommand stream: %1_%2 %3")
                .arg(provider_id)
                .arg(command_id)
                .arg(static_cast<int>(stream.type));
    qDebug() << stream.data;
}

void CFileDataAggregator::onCommandError(const QString provider_id,
                                         const QString command_id,
                                         const std::error_code error_code)
{
    qDebug() << QString("Command error: %1_%2 - %3")
                .arg(provider_id)
                .arg(command_id)
                .arg(error_code.message().c_str());
}
