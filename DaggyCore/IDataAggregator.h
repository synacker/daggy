/*
Copyright 2017-2020 Milovidov Mikhail

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#pragma once

#include <QObject>

#include "daggycore_export.h"
#include "Command.h"
#include "Result.h"

namespace daggycore {

class DAGGYCORE_EXPORT IDataAggregator : public QObject
{
    Q_OBJECT
public:
    explicit IDataAggregator(QObject *parent = nullptr);
    virtual ~IDataAggregator() = default;

    virtual Result prepare() = 0;
    virtual Result free() = 0;

public slots:
    virtual void onDataProviderStateChanged(const QString provider_id,
                                            const int state) = 0;
    virtual void onDataProviderError(const QString provider_id,
                                     const std::error_code error_code) = 0;

    virtual void onCommandStateChanged(const QString provider_id,
                                       const QString command_id,
                                       const Command::State state,
                                       const int exit_code) = 0;
    virtual void onCommandStream(const QString provider_id,
                                 const QString command_id,
                                 const Command::Stream stream) = 0;
    virtual void onCommandError(const QString provider_id,
                                const QString command_id,
                                const std::error_code error_code) = 0;

};

}
