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

#include <memory>
#include <DaggyCore/IDataAggregator.h>

#include <QMetaEnum>

class QFile;

class CFileDataAggregator : public daggycore::IDataAggregator
{
    Q_OBJECT
public:
    enum ConsoleMessageType {CommError, CommStat, ProvError, ProvStat, AppStat};
    Q_ENUM(ConsoleMessageType)

    CFileDataAggregator(QString output_folder,
                        QString data_sources_name,
                        QObject* parent = nullptr);
    ~CFileDataAggregator();

    daggycore::Result prepare() override;
    daggycore::Result free() override;

public slots:
    void onDataProviderStateChanged(const QString provider_id,
                                    const int state) override;
    void onDataProviderError(const QString provider_id,
                             const std::error_code error_code) override;

    void onCommandStateChanged(const QString provider_id,
                               const QString command_id,
                               const daggycore::Command::State state,
                               const int exit_code) override;
    void onCommandStream(const QString provider_id,
                         const QString command_id,
                         const daggycore::Command::Stream stream) override;
    void onCommandError(const QString provider_id,
                        const QString command_id,
                        const std::error_code error_code) override;

protected:
    void printAppMessage
    (
        const QString& message
    );
    void printProviderMessage
    (
        const ConsoleMessageType& message_type,
        const QString& provider_id,
        const QString& source_message
    );
    void printCommandMessage
    (
        const ConsoleMessageType& message_type,
        const QString& provider_id,
        const QString& command_id,
        const QString& command_message
    );

    void printMessage
    (
        const ConsoleMessageType& message_type,
        const QString& provider_id,
        const QString& command_id,
        const QString& command_message
    );

    QString currentConsoleTime() const;

private:
    bool writeToFile
    (
        const QString& provider_id,
        const QString& command_id,
        const daggycore::Command::Stream& stream
    );

    const QString output_folder_;
    const QString data_sources_name_;
    const QMetaEnum console_message_type_;
    const QMetaEnum provider_state_;
    const QMetaEnum command_state_;

    QMap<QString, std::shared_ptr<QFile>> stream_files_;
};

