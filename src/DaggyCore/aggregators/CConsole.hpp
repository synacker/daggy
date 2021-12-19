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
#include "IAggregator.hpp"

namespace daggy {
namespace aggregators {

class DAGGYCORE_EXPORT CConsole : public IAggregator
{
    Q_OBJECT
public:
    enum ConsoleMessageType {CommError, CommStat, ProvError, ProvStat, AppStat};
    Q_ENUM(ConsoleMessageType)

    CConsole(QString name,
             QObject* parent);

    bool isReady() const override;

public slots:
    void onDataProviderStateChanged(QString provider_id, DaggyProviderStates state) override;
    void onDataProviderError(QString provider_id, std::error_code error_code) override;
    void onCommandStateChanged(QString provider_id, QString command_id, DaggyCommandStates state, int exit_code) override;
    void onCommandError(QString provider_id, QString command_id, std::error_code error_code) override;
    void onCommandStream(QString provider_id, QString command_id, sources::commands::Stream stream) override;
    void onDaggyStateChanged(DaggyStates state) override;

private:
    QString stateName(DaggyProviderStates state) const;
    QString stateName(DaggyCommandStates state) const;


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

    const QString name_;

    const QMetaEnum console_message_type_;
};

}
}


