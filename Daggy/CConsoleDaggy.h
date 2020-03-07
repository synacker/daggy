/*
Copyright 2017-2020 Milovidov Mikhail

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#pragma once

#include <QObject>

#include <DaggyCore/Result.h>

#include "ISystemSignalHandler.h"

class QCoreApplication;

namespace daggycore {
class DaggyCore;
}

class CConsoleDaggy : public QObject,
                      public ISystemSignalHandler
{
    Q_OBJECT
public:
    CConsoleDaggy(QCoreApplication* application);

    daggycore::Result initialize();
    bool start();

    bool handleSystemSignal(const int signal);

    bool isError() const;
    const QString& errorMessage() const;

signals:
    void interrupt(const int signal);

private:
    struct Settings {
        QString data_source_text_type;
        QString data_source_text;
        QString output_folder;
    };
    Settings parse() const;

    daggycore::DaggyCore* daggyCore() const;
    QCoreApplication* application() const;

    QString getTextFromFile(QString file_path) const;
    QString generateOutputFolder(const QString& data_sources_name) const;
    QString homeFolder() const;

    daggycore::DaggyCore* daggy_core_;

    QString error_message_;
};

