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
#pragma once

#include <QObject>
#include <QThread>

#include <DaggyCore/Types.hpp>

namespace daggy {
namespace aggregators {
class CConsole;
}
}

class QCoreApplication;

namespace daggy {
class Core;
}

class CConsoleDaggy : public QObject
{
    Q_OBJECT
public:
    CConsoleDaggy(QObject* parent = nullptr);
    ~CConsoleDaggy();

    std::error_code prepare();
    std::error_code start();
    void stop();

    bool isError() const;
    const QString& errorMessage() const;

signals:
    void interrupt(const int signal);

private slots:
    void onDaggyCoreStateChanged(DaggyStates state);
    void checkSignalStatus();

private:
    bool registrateSignalsHandler();

    const QVector<QString>& supportedConvertors() const;
    DaggySourcesTextTypes textFormatType(const QString& file_name) const;

    struct Settings {
        DaggySourcesTextTypes data_source_text_type;
        QString data_source_text;
        QString output_folder;
        QString data_sources_name;
        unsigned int timeout = 0;
        bool fix_pcap = false;
    };
    Settings parse() const;
    void fixPcaps() const;

    daggy::Core* daggyCore() const;
    QCoreApplication* application() const;

    QString getTextFromFile(QString file_path) const;
    QString homeFolder() const;

    QString mustache(const QString& text, const QString& output_folder, const QMap<QString, QString> &env_parameters) const;

    Settings settings_;
    QString session_;
    QThread file_thread_;

    daggy::Core* daggy_core_;
    daggy::aggregators::CConsole* console_aggreagator_;
    bool need_hard_stop_;

    QString error_message_;
};

