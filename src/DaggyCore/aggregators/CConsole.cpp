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
#include "CConsole.hpp"
#include "../Types.hpp"

daggy::aggregators::CConsole::CConsole(QString name, QObject* parent)
    : IAggregator(parent)
    , name_(std::move(name))
    , console_message_type_(QMetaEnum::fromType<ConsoleMessageType>())
{

}

bool daggy::aggregators::CConsole::isReady() const
{
    return true;
}

void daggy::aggregators::CConsole::onDataProviderStateChanged(QString provider_id, DaggyProviderStates state)
{
    const auto& provider_state_string = stateName(state);
    const QString message = QString("New state: %1").arg(provider_state_string);
    printProviderMessage
    (
        ConsoleMessageType::ProvStat,
        provider_id,
        message
    );
}

void daggy::aggregators::CConsole::onDataProviderError(QString provider_id, std::error_code error_code)
{
    printProviderMessage
    (
        ProvError,
        provider_id,
        QString::fromStdString(error_code.message())
    );
}

void daggy::aggregators::CConsole::onCommandStateChanged(QString provider_id, QString command_id, DaggyCommandStates state, int exit_code)
{
    const auto& state_string = stateName(state);
    QString message = QString("New state: %1").arg(state_string);
    if (state == DaggyCommandFinished)
        message += QString(". Exit code: %1").arg(exit_code);
    printCommandMessage
    (
        CommStat,
        provider_id,
        command_id,
        message
    );
}

void daggy::aggregators::CConsole::onCommandError(QString provider_id, QString command_id, std::error_code error_code)
{
    printCommandMessage
    (
        CommStat,
        provider_id,
        command_id,
        QString::fromStdString(error_code.message())
    );
}

void daggy::aggregators::CConsole::onCommandStream(QString provider_id, QString command_id, sources::commands::Stream stream)
{

}

void daggy::aggregators::CConsole::onDaggyStateChanged(DaggyStates state)
{
    switch (state) {
    case DaggyNotStarted:
        break;
    case DaggyStarted:
        printAppMessage(QString("Start aggregation %1").arg(name_));
        break;
    case DaggyFinishing:
        break;
    case DaggyFinished:
        printAppMessage(QString("Stop aggregation %1").arg(name_));
        break;
    }
}

QString daggy::aggregators::CConsole::stateName(DaggyProviderStates state) const
{
    QString result;
    switch (state) {
    case DaggyProviderNotStarted:
        result = "Not started";
        break;
    case DaggyProviderStarting:
        result = "Starting";
        break;
    case DaggyProviderFailedToStart:
        result = "Failed to start";
        break;
    case DaggyProviderStarted:
        result = "Started";
        break;
    case DaggyProviderFinishing:
        result = "Finishing";
        break;
    case DaggyProviderFinished:
        result = "Finished";
        break;
    }
    return result;
}

QString daggy::aggregators::CConsole::stateName(DaggyCommandStates state) const
{
    QString result;
    switch (state) {
    case DaggyCommandNotStarted:
        result = "Not started";
        break;
    case DaggyCommandStarting:
        result = "Starting";
        break;
    case DaggyCommandStarted:
        result = "Started";
        break;
    case DaggyCommandFailedToStart:
        result = "Failed to start";
        break;
    case DaggyCommandFinishing:
        result = "Finishing";
        break;
    case DaggyCommandFinished:
        result = "Finished";
        break;
    }
    return result;
}

void daggy::aggregators::CConsole::printAppMessage(const QString& message)
{
    printMessage(AppStat, QString(), QString(), message);
}

void daggy::aggregators::CConsole::printProviderMessage(const ConsoleMessageType& message_type, const QString& provider_id, const QString& source_message)
{
    printMessage(message_type, provider_id, QString(), source_message);
}

void daggy::aggregators::CConsole::printCommandMessage(const ConsoleMessageType& message_type, const QString& provider_id, const QString& command_id, const QString& command_message)
{
    printMessage(message_type, provider_id, command_id, command_message);
}

void daggy::aggregators::CConsole::printMessage(const ConsoleMessageType& message_type, const QString& provider_id, const QString& command_id, const QString& command_message)
{
    const char* message_type_string = console_message_type_.valueToKey(message_type);
    printf
    (
           "%12s | %-9s | %-20s | %-30s | %s\n",
           qPrintable(currentConsoleTime()),
           message_type_string,
           qPrintable(provider_id),
           qPrintable(command_id),
           qPrintable(command_message)
    );
}

QString daggy::aggregators::CConsole::currentConsoleTime() const
{
    return QDateTime::currentDateTime().toString("hh:mm:ss:zzz");
}
