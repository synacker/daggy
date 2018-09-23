#include "Precompiled.h"
#include "IRemoteServer.h"

IRemoteServer::IRemoteServer(const QString& serverName,
                             const QString& connectionType,
                             const QVector<RemoteCommand>& commands, QObject* pParent)
    : QObject(pParent)
    , m_serverName(serverName)
    , m_connectionType(connectionType)
    , m_remoteCommands(convertRemoteCommands(commands))
{
    setObjectName(serverName);
    for (const QString& commandName : m_remoteCommands.keys()) {
        m_commandsStatus[commandName] = RemoteCommand::Status::NotStarted;
    }
}

IRemoteServer::~IRemoteServer()
{
    qDeleteAll(m_remoteCommands.values());
}

QString IRemoteServer::serverName() const
{
    return m_serverName;
}

void IRemoteServer::startCommands()
{
    for (const RemoteCommand* const pRemoteCommand : m_remoteCommands.values()) {
        if (commandStatus(pRemoteCommand->commandName) != RemoteCommand::Status::Started)
            startCommand(pRemoteCommand->commandName);
    }
}

void IRemoteServer::setConnectionStatus(const IRemoteServer::ConnectionStatus status, const QString& message)
{
    m_connectionStatus = status;
    emit connectionStatusChanged(m_serverName, status, message);
}

void IRemoteServer::setRemoteCommandStatus(QString commandName, RemoteCommand::Status status, int exitCode)
{
    m_commandsStatus[commandName] = status;
    emit remoteCommandStatusChanged(m_serverName, commandName, status, exitCode);
}

void IRemoteServer::setNewRemoteCommandStream(const QString& commandName, const QByteArray& data, const RemoteCommand::Stream::Type type)
{
    const RemoteCommand* const pRemoteCommand = getRemoteCommand(commandName);
    emit newRemoteCommandStream(m_serverName, {commandName, pRemoteCommand->outputExtension, data, type});
}

QMap<QString, RemoteCommand*> IRemoteServer::convertRemoteCommands(const QVector<RemoteCommand>& remoteCommands) const
{
    QMap<QString, RemoteCommand*> result;

    for (const RemoteCommand& remoteCommand : remoteCommands) {
        result.insert(remoteCommand.commandName, new RemoteCommand(remoteCommand));
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

RemoteCommand::Status IRemoteServer::commandStatus(const QString& commandName) const
{
    return m_commandsStatus.value(commandName, RemoteCommand::Status::NotStarted);
}

const RemoteCommand* IRemoteServer::getRemoteCommand(const QString& commandName) const
{
    return m_remoteCommands.value(commandName, nullptr);
}

bool IRemoteServer::isExistsRunningRemoteCommands() const
{
    return runingRemoteCommandsCount() > 0;
}
