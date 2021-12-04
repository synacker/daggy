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

using namespace daggy;

CLocalDataProvider::CLocalDataProvider
(
    Commands commands,
    QObject *parent
)
: IDataProvider(commands, parent)
{
}

CLocalDataProvider::~CLocalDataProvider()
{
    for (auto process : processes()) {
        process->kill();
    }

    for (auto process : processes()) {
        process->waitForFinished();
    }
}

void CLocalDataProvider::start()
{
    switch (state()) {
    case daggy::IDataProvider::NotStarted:
    case daggy::IDataProvider::FailedToStart:
    case daggy::IDataProvider::Finished:
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
    terminate();
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
    const size_t active_processes = activeProcessesCount();
    switch (state()) {
    case daggy::IDataProvider::NotStarted:
        break;
    case daggy::IDataProvider::Starting:
        if (activeProcessesCount() == 0)
            setState(Finished);
        break;
    case daggy::IDataProvider::FailedToStart:
        break;
    case daggy::IDataProvider::Started:
        if (active_processes == 0 && restartCommandsCount() == 0)
            setState(Finished);
        break;
    case daggy::IDataProvider::Finishing:
        if (active_processes == 0)
            setState(Finished);
        break;
    case daggy::IDataProvider::Finished:
        break;
    }
}

void CLocalDataProvider::onProcessStart()
{
    auto process = qobject_cast<QProcess*>(sender());

    emit commandStateChanged(process->objectName(),
                             Command::Started,
                             process->exitCode());
}

void CLocalDataProvider::onProcessError(QProcess::ProcessError error)
{
    auto process = qobject_cast<QProcess*>(sender());
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
    onProcessStop(process);
}

void CLocalDataProvider::onProcessReadyReadStandard()
{
    onProcessReadyReadStandard(qobject_cast<QProcess*>(sender()));
}

void CLocalDataProvider::terminate()
{
    if (state() != Started)
        return;

    if (activeProcessesCount() > 0) {
        setState(Finishing);
        for (QProcess* process : processes()) {
            switch (process->state()) {
            case QProcess::Running:
#ifdef Q_OS_WIN
                process->kill();
#else
                process->terminate();
#endif
                break;
            case QProcess::Starting:
                process->close();
                break;
            default:;
            }
        }
    }
    else
        setState(Finished);
}

QProcess* CLocalDataProvider::startProcess(const Command &command)
{
    QProcess* process = new QProcess(this);
    process->setObjectName(command.id);

    connect(process, &QProcess::destroyed, this, &CLocalDataProvider::onProcessDestroyed);
    connect(process, &QProcess::started, this, &CLocalDataProvider::onProcessStart);
    connect(process, &QProcess::errorOccurred, this, &CLocalDataProvider::onProcessError);
    connect(process, &QProcess::readyReadStandardOutput, this, qOverload<>(&CLocalDataProvider::onProcessReadyReadStandard));
    connect(process, &QProcess::readyReadStandardError, this,  qOverload<>(&CLocalDataProvider::onProcessReadyReadError));
    connect(process, &QProcess::finished, this, &CLocalDataProvider::onProcessFinished);

    auto parameters = command.exec.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
    auto program = parameters.takeFirst();
    emit commandStateChanged(process->objectName(),
                             Command::Starting,
                             process->exitCode());
    process->start(program, parameters);
    return process;
}

bool CLocalDataProvider::onProcessStop(QProcess *process)
{
    onProcessReadyReadStandard(process);
    onProcessReadyReadError(process);
    process->deleteLater();
    const auto& command = commands().value(process->objectName());
    if (command.restart && state() == IDataProvider::Started) {
        startProcess(command);
        return true;
    }
    return false;
}

void CLocalDataProvider::startCommands()
{
    for (const auto& command : commands()) {
        startProcess(command);
    }
}

void CLocalDataProvider::startProcess(QProcess *process, const QString& command)
{
    auto parameters = command.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
    auto program = parameters.takeFirst();
    emit commandStateChanged(process->objectName(),
                             Command::Starting,
                             process->exitCode());
    process->start(program, parameters, QIODevice::ReadOnly);
}

void CLocalDataProvider::onProcessReadyReadStandard(QProcess* process)
{
    if (process == nullptr)
        process = qobject_cast<QProcess*>(sender());
    auto command = commands().value(process->objectName());
    auto stream = process->readAllStandardOutput();
    if (stream.isEmpty())
        return;
    emit commandStream
    (
        process->objectName(),
        {
            command.extension,
            stream,
            Command::Stream::Type::Standard
        }
                );
}

void CLocalDataProvider::onProcessReadyReadError()
{
    onProcessReadyReadError(qobject_cast<QProcess*>(sender()));
}

void CLocalDataProvider::onProcessReadyReadError(QProcess* process)
{
    if (process == nullptr)
        process = qobject_cast<QProcess*>(sender());
    auto command = commands().value(process->objectName());
    auto stream = process->readAllStandardError();
    if (stream.isEmpty())
        return;
    emit commandStream
    (
        process->objectName(),
        {
            command.extension,
            stream,
            Command::Stream::Type::Error
        }
    );
}

void CLocalDataProvider::onProcessFinished(int exit_code, QProcess::ExitStatus)
{
    auto process = qobject_cast<QProcess*>(sender());
    emit commandStateChanged(process->objectName(),
                             Command::Finished,
                             exit_code);
    onProcessStop(process);
}

