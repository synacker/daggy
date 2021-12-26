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
#include "CLocal.hpp"
#include "../Errors.hpp"

const QString daggy::providers::CLocal::provider_type("local");

daggy::providers::CLocal::CLocal(const QString& session, sources::Commands commands,
                                 QObject* parent)
    : IProvider(session, std::move(commands), parent)
{

}

daggy::providers::CLocal::~CLocal()
{
    for (auto process : processes()) {
        process->kill();
    }

    for (auto process : processes()) {
        process->waitForFinished();
    }
}

std::error_code daggy::providers::CLocal::start() noexcept
{
    std::error_code result = errors::success;
    switch (state()) {
    case DaggyProviderNotStarted:
    case DaggyProviderFailedToStart:
    case DaggyProviderFinished:
    {
        setState(DaggyProviderStarted);
        startCommands();
    }
        break;
    default:
        result = errors::make_error_code(DaggyErrorProviderAlreadyStarted);
    }
    return result;
}

std::error_code daggy::providers::CLocal::stop() noexcept
{
    std::error_code result = errors::success;
    switch (state()) {
    case DaggyProviderNotStarted:
    case DaggyProviderFailedToStart:
    case DaggyProviderFinished:
        result = errors::make_error_code(DaggyErrorProviderAlreadyFinished);
        break;
    case DaggyProviderStarting:
    case DaggyProviderStarted:
    case DaggyProviderFinishing:
        terminate();
        break;

    }
    return result;
}

const QString& daggy::providers::CLocal::type() const noexcept
{
    return provider_type;
}

void daggy::providers::CLocal::onProcessDestroyed()
{
    const size_t active_processes = activeProcessesCount();
    switch (state()) {
    case DaggyProviderNotStarted:
        break;
    case DaggyProviderStarting:
        if (activeProcessesCount() == 0)
            setState(DaggyProviderFinished);
        break;
    case DaggyProviderFailedToStart:
        break;
    case DaggyProviderStarted:
        if (active_processes == 0 && restartCommandsCount() == 0)
            setState(DaggyProviderFinished);
        break;
    case DaggyProviderFinishing:
        if (active_processes == 0)
            setState(DaggyProviderFinished);
        break;
    case DaggyProviderFinished:
        break;
    }
}

void daggy::providers::CLocal::onProcessStart()
{
    auto process = qobject_cast<QProcess*>(sender());

    emit commandStateChanged(process->objectName(),
                             DaggyCommandStarted,
                             process->exitCode());
}

void daggy::providers::CLocal::onProcessError(QProcess::ProcessError error)
{
    auto process = qobject_cast<QProcess*>(sender());
    switch (error) {
    case QProcess::FailedToStart:
    case QProcess::Timedout:
        emit commandStateChanged(process->objectName(),
                                 DaggyCommandFailedToStart,
                                 process->exitCode());
        break;
    case QProcess::ReadError:
        emit commandError(process->objectName(),
                          errors::make_error_code(DaggyErrorCommandRead));
        break;
    default:;
    }
    onProcessStop(process);
}

void daggy::providers::CLocal::onProcessReadyReadStandard()
{
    onProcessReadyReadStandard(qobject_cast<QProcess*>(sender()));
}

void daggy::providers::CLocal::onProcessReadyReadStandard(QProcess* process)
{
    if (process == nullptr)
        process = qobject_cast<QProcess*>(sender());
    const auto& command = commands().value(process->objectName());
    auto stream = process->readAllStandardOutput();
    if (stream.isEmpty())
        return;
    const QString& id = process->objectName();
    emit commandStream
    (
        id,
        {
            metaStream(id, DaggyStreamTypes::DaggyStreamStandard),
            std::move(stream)
        }
    );
}

void daggy::providers::CLocal::onProcessReadyReadError()
{
    onProcessReadyReadError(qobject_cast<QProcess*>(sender()));
}

void daggy::providers::CLocal::onProcessReadyReadError(QProcess* process)
{
    if (process == nullptr)
        process = qobject_cast<QProcess*>(sender());
    auto command = commands().value(process->objectName());
    auto stream = process->readAllStandardError();
    if (stream.isEmpty())
        return;
    const auto& id = process->objectName();
    emit commandStream
    (
        id,
        {
            metaStream(id, DaggyStreamError),
            std::move(stream)
        }
    );
}

void daggy::providers::CLocal::onProcessFinished(int exit_code, QProcess::ExitStatus)
{
    auto process = qobject_cast<QProcess*>(sender());
    emit commandStateChanged(process->objectName(),
                             DaggyCommandFinished,
                             exit_code);
    onProcessStop(process);
}

void daggy::providers::CLocal::terminate()
{
    if (state() != DaggyProviderStarted)
        return;

    if (activeProcessesCount() > 0) {
        setState(DaggyProviderFinishing);
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
        setState(DaggyProviderFinished);
}

QProcess* daggy::providers::CLocal::startProcess(const sources::Command& command)
{
    QProcess* process = new QProcess(this);
    process->setObjectName(command.first);

    connect(process, &QProcess::destroyed, this, &CLocal::onProcessDestroyed);
    connect(process, &QProcess::started, this, &CLocal::onProcessStart);
    connect(process, &QProcess::errorOccurred, this, &CLocal::onProcessError);
    connect(process, &QProcess::readyReadStandardOutput, this, qOverload<>(&CLocal::onProcessReadyReadStandard));
    connect(process, &QProcess::readyReadStandardError, this,  qOverload<>(&CLocal::onProcessReadyReadError));
    connect(process, &QProcess::finished, this, &CLocal::onProcessFinished);

    const auto& properties = command.second;

    auto parameters = properties.exec.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
    auto program = parameters.takeFirst();
    emit commandStateChanged(process->objectName(),
                             DaggyCommandStarting,
                             process->exitCode());
    process->start(program, parameters);
    return process;
}

bool daggy::providers::CLocal::onProcessStop(QProcess* process)
{
    onProcessReadyReadStandard(process);
    onProcessReadyReadError(process);
    process->deleteLater();
    const auto& command_id = process->objectName();
    const auto& properties = commands().value(command_id);
    if (properties.restart && state() == DaggyProviderStarted) {
        startProcess({command_id, properties});
        return true;
    }
    return false;
}

QList<QProcess*> daggy::providers::CLocal::processes() const
{
    return findChildren<QProcess*>();
}

int daggy::providers::CLocal::activeProcessesCount() const
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

void daggy::providers::CLocal::startCommands()
{
    const auto& start_commands = commands();
    auto it = start_commands.begin();
    while (it != start_commands.end()) {
        startProcess({it.key(), it.value()});
        it++;
    }
}

void daggy::providers::CLocal::startProcess(QProcess* process, const QString& command)
{
    const auto& id = process->objectName();
    auto parameters = command.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
    auto program = parameters.takeFirst();
    metaStream(id, DaggyStreamStandard, true);
    metaStream(id, DaggyStreamError, true);
    emit commandStateChanged(id,
                             DaggyCommandStarting,
                             process->exitCode());
    process->start(program, parameters, QIODevice::ReadOnly);
}
