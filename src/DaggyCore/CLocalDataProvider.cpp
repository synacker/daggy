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
#include "Precompiled.h"
#include "CLocalDataProvider.h"

#include "Common.h"

using namespace daggycore;

CLocalDataProvider::CLocalDataProvider
(
    Commands commands,
    QObject *parent
)
: IDataProvider(commands, parent)
{
}

void CLocalDataProvider::start()
{
    switch (state()) {
    case daggycore::IDataProvider::NotStarted:
    case daggycore::IDataProvider::FailedToStart:
    case daggycore::IDataProvider::Finished:
    {
        setState(Started);
        startCommands();
    }
        break;
    default:;
    }
}

void CLocalDataProvider::stop()
{
    if (activeProcessesCount() > 0) {
        setState(Finishing);
        for (QProcess* process : processes())
            process->terminate();
    }
    else
        setState(Finished);
}

QString CLocalDataProvider::type() const
{
    return provider_type;
}

QList<QProcess*> CLocalDataProvider::processes() const
{
    return findChildren<QProcess*>();
}

int CLocalDataProvider::activeProcessesCount() const
{
    int result = 0;
    for (QProcess* process : processes()) {
        switch (process->state()) {
        case QProcess::NotRunning:
            break;
        case QProcess::Starting:
        case QProcess::Running:
            result++;
            break;
        }
    }

    return result;
}

void CLocalDataProvider::onProcessDestroyed()
{
    if (
            (state() == Finishing && activeProcessesCount() == 0) ||
            (state() == Started && activeProcessesCount() == 0 && restartCommandsCount() == 0)
    )
    {
        setState(Finished);
    }
}

void CLocalDataProvider::startCommands()
{
    for (const auto& command : commands()) {
        QProcess* process = new QProcess(this);
        process->setObjectName(command.id);

        connect(process, &QProcess::destroyed, this, &CLocalDataProvider::onProcessDestroyed);

        connect(process, &QProcess::stateChanged, this,
        [=](QProcess::ProcessState state)
        {
            Command::State command_state = Command::NotStarted;
            switch (state) {
            case QProcess::Starting:
                command_state = Command::Starting;
                break;
            case QProcess::Running:
                command_state = Command::Started;
                break;
            default:;
            }
            if (command_state != Command::NotStarted)
            {
                emit commandStateChanged(process->objectName(),
                                         command_state,
                                         process->exitCode());
            }
        }
        );
        connect(process, &QProcess::errorOccurred, this,
        [=](QProcess::ProcessError error) {
            switch (error) {
            case QProcess::FailedToStart:
            case QProcess::Timedout:
                emit commandStateChanged(process->objectName(),
                                         Command::FailedToStart,
                                         process->exitCode());
                break;
            case QProcess::ReadError:
                emit commandError(process->objectName(),
                                  DaggyErrors::CommandReadError);
                break;
            default:;
            }
        });
        connect(process, &QProcess::readyReadStandardOutput, this,
        [=]()
        {
            emit commandStream
            (
                process->objectName(),
                {
                    command.extension,
                    process->readAllStandardOutput(),
                    Command::Stream::Type::Standard
                }
            );
        });
        connect(process, &QProcess::readyReadStandardError, this,
        [=]()
        {
            emit commandStream
            (
                process->objectName(),
                {
                    command.extension,
                    process->readAllStandardError(),
                    Command::Stream::Type::Error
                }
            );
        });

        connect(process, qOverload<int, QProcess::ExitStatus>(&QProcess::finished), this,
        [=]()
        {
            emit commandStream
            (
                process->objectName(),
                {
                    command.extension,
                    process->readAllStandardError(),
                    Command::Stream::Type::Error
                }
            );
            emit commandStream
            (
                process->objectName(),
                {
                    command.extension,
                    process->readAllStandardOutput(),
                    Command::Stream::Type::Standard
                }
            );
        });

        connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this,
            [=](int exit_code, QProcess::ExitStatus)
            {
                emit commandStateChanged(process->objectName(),
                                         Command::Finished,
                                         exit_code);
                if (command.restart && state() == IDataProvider::Started)
                    process->start(command.exec);
                else
                    process->deleteLater();
            }
        );
        process->start(command.exec);
    }
}
