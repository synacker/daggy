#include "Precompiled.h"
#include "CLocalRemoteServer.h"

CLocalRemoteServer::CLocalRemoteServer(const DataSource& data_source,
                                       QObject* parent_pointer)
    : IRemoteServer(data_source, parent_pointer)
{

}

void CLocalRemoteServer::startAgregator()
{
    setConnectionStatus(RemoteConnectionStatus::Connected);
}

void CLocalRemoteServer::stopAgregator()
{
    for (QProcess* process_ptr : processes()) {
        process_ptr->close();
        process_ptr->deleteLater();
    }
    setConnectionStatus(RemoteConnectionStatus::Disconnected);
}

void CLocalRemoteServer::restartCommand(const QString& command_name)
{
    QProcess* process_ptr = process(command_name);
    if (process_ptr) {
        process_ptr->close();
        process_ptr->deleteLater();
    }

    process_ptr = new QProcess(this);
    process_ptr->setObjectName(command_name);

    const RemoteCommand& remote_command = getRemoteCommand(command_name);

    connect(process_ptr, &QProcess::errorOccurred,
            this, &CLocalRemoteServer::onProcessError);
    connect(process_ptr,
            static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
            this, &CLocalRemoteServer::onProcessFinished);

    connect(process_ptr, &QProcess::readyReadStandardError,
            this, &CLocalRemoteServer::onReadyReadStandardError);
    connect(process_ptr, &QProcess::readyReadStandardOutput,
            this, &CLocalRemoteServer::onReadyReadStandardOutput);

    connect(process_ptr, &QProcess::stateChanged,
            this, &CLocalRemoteServer::onProcessStateChanged);

    process_ptr->start(remote_command.command);
}

void CLocalRemoteServer::reconnect()
{

}

void CLocalRemoteServer::onProcessError(const QProcess::ProcessError error)
{
    const QString& command_name = sender()->objectName();
    switch (error) {
    case QProcess::FailedToStart:
        setRemoteCommandStatus(command_name, RemoteCommand::Status::FailedToStart);
        break;
    default:;
    }
}

void CLocalRemoteServer::onProcessFinished(const int exit_code, const QProcess::ExitStatus exit_status)
{
    const QString& command_name = sender()->objectName();
    RemoteCommand::Status command_status = RemoteCommand::Status::NotStarted;
    switch (exit_status) {
    case QProcess::NormalExit:
        command_status = RemoteCommand::Status::NormalExit;
        break;
    case QProcess::CrashExit:
        command_status = RemoteCommand::Status::CrashExit;
        break;
    }
    setRemoteCommandStatus(command_name, command_status, exit_code);
}

QList<QProcess*> CLocalRemoteServer::processes() const
{
    return findChildren<QProcess*>();
}

QProcess* CLocalRemoteServer::process(const QString& command_name) const
{
    return findChild<QProcess*>(command_name);
}

void CLocalRemoteServer::onReadyReadStandardError()
{
    const QString& command_name = sender()->objectName();
    QProcess* const process_ptr = process(command_name);
    if (process_ptr)
        setNewRemoteCommandStream(command_name, process_ptr->readAllStandardError(), RemoteCommand::Stream::Type::Error);
}

void CLocalRemoteServer::onReadyReadStandardOutput()
{
    const QString& command_name = sender()->objectName();
    QProcess* const process_ptr = process(command_name);
    if (process_ptr)
        setNewRemoteCommandStream(command_name, process_ptr->readAllStandardOutput(), RemoteCommand::Stream::Type::Standard);
}

void CLocalRemoteServer::onProcessStateChanged(const QProcess::ProcessState state)
{
    const QString& command_name = sender()->objectName();
    if (state == QProcess::Running)
        setRemoteCommandStatus(command_name, RemoteCommand::Status::Started);
}
