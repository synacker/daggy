/*
Copyright 2017-2020 Milovidov Mikhail

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "Precompiled.h"
#include "CFileDataAggregator.h"

#include <DaggyCore/IDataProvider.h>

CFileDataAggregator::CFileDataAggregator(QString output_folder, QObject* parent)
    : daggycore::IDataAggregator(parent)
    , output_folder_(std::move(output_folder))
    , console_message_type_(QMetaEnum::fromType<ConsoleMessageType>())
    , provider_state_(QMetaEnum::fromType<daggycore::IDataProvider::State>())
    , command_state_(QMetaEnum::fromType<daggycore::Command::State>())
{
    printAppMessage("Start aggregation");
}

CFileDataAggregator::~CFileDataAggregator()
{
    printAppMessage("Stop aggregation");
}

void CFileDataAggregator::onDataProviderStateChanged(const QString provider_id,
                                                     const int state)
{
    const auto provider_state = static_cast<daggycore::IDataProvider::State>(state);
    const char* provider_state_string = provider_state_.valueToKey(provider_state);
    const QString message = QString("New state: %1").arg(provider_state_string);
    printProviderMessage
    (
        ConsoleMessageType::ProvStat,
        provider_id,
        message
    );
}

void CFileDataAggregator::onDataProviderError(const QString provider_id,
                                              const std::error_code error_code)
{
    printProviderMessage
    (
        ProvError,
        provider_id,
        QString::fromStdString(error_code.message())
    );
}

void CFileDataAggregator::onCommandStateChanged(const QString provider_id,
                                                const QString command_id,
                                                const daggycore::Command::State state,
                                                const int exit_code)
{
    const char* state_string = command_state_.valueToKey(state);
    QString message = QString("New state: %1").arg(state_string);
    if (state == daggycore::Command::Finished)
        message += QString(". Exit code: %1").arg(exit_code);
    printCommandMessage
    (
        CommStat,
        provider_id,
        command_id,
        message
    );
}

void CFileDataAggregator::onCommandStream(const QString provider_id,
                                          const QString command_id,
                                          const daggycore::Command::Stream stream)
{
 /*   qDebug() << QString("\nCommand stream: %1_%2 %3")
                .arg(provider_id)
                .arg(command_id)
                .arg(static_cast<int>(stream.type));
    qDebug() << stream.data;*/
}

void CFileDataAggregator::onCommandError(const QString provider_id,
                                         const QString command_id,
                                         const std::error_code error_code)
{
    printCommandMessage
    (
        CommStat,
        provider_id,
        command_id,
        QString::fromStdString(error_code.message())
    );
}

void CFileDataAggregator::printAppMessage(const QString& message)
{
    const char* message_type_string = console_message_type_.valueToKey(AppStat);
    printf
    (
           "%12s | %-8s | %s\n",
           qPrintable(currentConsoleTime()),
           message_type_string,
           qPrintable(message)
    );
}

void CFileDataAggregator::printProviderMessage
(
        const CFileDataAggregator::ConsoleMessageType& message_type,
        const QString& provider_id,
        const QString& source_message
)
{
    const char* message_type_string = console_message_type_.valueToKey(message_type);
    printf
    (
           "%12s | %-8s | %-15s | %s\n",
           qPrintable(currentConsoleTime()),
           message_type_string,
           qPrintable(provider_id),
           qPrintable(source_message)
    );
}

void CFileDataAggregator::printCommandMessage
(
    const CFileDataAggregator::ConsoleMessageType& message_type,
    const QString& provider_id,
    const QString& command_id,
    const QString& command_message)
{
    const char* message_type_string = console_message_type_.valueToKey(message_type);
    printf
    (
           "%12s | %-8s | %-15s | %-15s | %s\n",
           qPrintable(currentConsoleTime()),
           message_type_string,
           qPrintable(provider_id),
           qPrintable(command_id),
           qPrintable(command_message)
    );
}

QString CFileDataAggregator::currentConsoleTime() const
{
    return QDateTime::currentDateTime().toString("hh:mm:ss:zzz");
}
