#include "Precompiled.h"
#include "IRemoteServer.h"

IRemoteServer::IRemoteServer(const QString& serverName,
                             const QString& connectionType,
                             const QVector<RemoteCommand>& commands, QObject* pParent)
    : QObject(pParent)
    , m_serverName(serverName)
    , m_connectionType(connectionType)
    , m_remoteCommands(convert(commands))
{
    setObjectName(serverName);
    for (const QString& commandName : m_remoteCommands.keys()) {
        m_commandsStatus[commandName] = RemoteCommand::Status::NotStarted;
    }
}

QString IRemoteServer::serverName() const
{
    return m_serverName;
}

void IRemoteServer::startCommands()
{
    for (const RemoteCommand& remoteCommand : m_remoteCommands.values()) {
        if (commandStatus(remoteCommand.commandName) != RemoteCommand::Status::Started)
            startCommand(remoteCommand.commandName);
    }
}

void IRemoteServer::setConnectionStatus(const IRemoteServer::ConnectionStatus status, const QString& message)
{
    m_connectionStatus = status;
    emit connectionStatusChanged(m_serverName, status, message);
}

void IRemoteServer::setRemoteCommandStatus(QString commandName, IRemoteServer::RemoteCommand::Status status, int statusCode)
{
    m_commandsStatus[commandName] = status;
    emit remoteCommandStatusChanged(m_serverName, commandName, status, statusCode);
}

void IRemoteServer::setNewRemoteCommandStream(const QString& commandName, const QByteArray& data, const IRemoteServer::RemoteCommand::Stream::Type type)
{
    const RemoteCommand& command = remoteCommand(commandName);
    emit newRemoteCommandStream(m_serverName, {commandName, command.outputExtension, data, type});
}

QMap<QString, IRemoteServer::RemoteCommand> IRemoteServer::convert(const QVector<IRemoteServer::RemoteCommand>& remoteCommands) const
{
    QMap<QString, IRemoteServer::RemoteCommand> result;

    for (const IRemoteServer::RemoteCommand& remoteCommand : remoteCommands) {
        result.insert(remoteCommand.commandName, remoteCommand);
    }

    return result;
}

QString IRemoteServer::connectionType() const
{
    return m_connectionType;
}

IRemoteServer::ConnectionStatus IRemoteServer::connectionStatus() const
{
    return m_connectionStatus;
}

IRemoteServer::RemoteCommand::Status IRemoteServer::commandStatus(const QString& commandName) const
{
    return m_commandsStatus.value(commandName, RemoteCommand::Status::NotStarted);
}

IRemoteServer::RemoteCommand IRemoteServer::remoteCommand(const QString& commandName) const
{
    return m_remoteCommands.value(commandName);
}
