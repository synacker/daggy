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
#pragma once

#include "../daggycore_export.h"
#include <QObject>
#include <system_error>

#include "../Sources.hpp"

namespace daggy {
namespace aggregators {

class DAGGYCORE_EXPORT IAggregator : public QObject
{
    Q_OBJECT
public:
    IAggregator(QObject* parent = nullptr);
    virtual ~IAggregator();

    virtual bool isReady() const = 0;

public slots:
    virtual void onDataProviderStateChanged(QString provider_id,
                                            DaggyProviderStates state) = 0;
    virtual void onDataProviderError(QString provider_id,
                                     std::error_code error_code) = 0;

    virtual void onCommandStateChanged(QString provider_id,
                                       QString command_id,
                                       DaggyCommandStates state,
                                       int exit_code) = 0;

    virtual void onCommandError(QString provider_id,
                                QString command_id,
                                std::error_code error_code) = 0;

    virtual void onCommandStream(QString provider_id,
                                 QString command_id,
                                 sources::commands::Stream stream) = 0;

    virtual void onDaggyStateChanged(DaggyStates state) = 0;

};

}
}
