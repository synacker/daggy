#include "Precompiled.h"
#include "IRemoteServer.h"

IRemoteServer::IRemoteServer(const QString& serverName,
                             const QString& connectionType,
                             const std::vector<RemoteCommand>& commands, QObject* pParent)
    : IRemoteAgregator(pParent)
    , m_serverName(serverName)
    , m_connectionType(connectionType)
    , m_remoteCommands(convertRemoteCommands(commands))
{
    setObjectName(serverName);
    for (const auto& pair : m_remoteCommands) {
        m_commandsStatus[pair.first] = RemoteCommand::Status::NotStarted;
    }
}

IRemoteServer::~IRemoteServer()
{
}

QString IRemoteServer::serverName() const
{
    return m_serverName;
}

void IRemoteServer::startCommands()
{
    for (const auto& pair : m_remoteCommands) {
        const QString& commandName = pair.first;
        if (commandStatus(commandName) != RemoteCommand::Status::Started)
            restartCommand(commandName);
    }
}

void IRemoteServer::setConnectionStatus(const IRemoteServer::ConnectionStatus status, const QString& message)
{
    m_connectionStatus = status;
    emit connectionStatusChanged(m_serverName, status, message);
    if (status == ConnectionStatus::Connected)
        startCommands();
    else
        setState(State::Stopped);
}

void IRemoteServer::setRemoteCommandStatus(const QString& commandName, RemoteCommand::Status commandStatus, const int exitCode)
{
    m_commandsStatus[commandName] = commandStatus;
    emit remoteCommandStatusChanged(m_serverName, commandName, commandStatus, exitCode);
    const RemoteCommand& remoteCommand = getRemoteCommand(commandName);
    if (state() == State::Run &&
        commandStatus != RemoteCommand::Status::Started &&
        remoteCommand.restart)
    {
        restartCommand(commandName);
    }
}

void IRemoteServer::setNewRemoteCommandStream(const QString& commandName, const QByteArray& data, const RemoteCommand::Stream::Type type)
{
    const RemoteCommand& pRemoteCommand = getRemoteCommand(commandName);
    emit newRemoteCommandStream(m_serverName, {commandName, pRemoteCommand.outputExtension, data, type});
}

std::map<QString, RemoteCommand> IRemoteServer::convertRemoteCommands(const std::vector<RemoteCommand>& remoteCommands) const
{
    std::map<QString, RemoteCommand> result;

    for (const RemoteCommand& remoteCommand : remoteCommands) {
        result.insert({remoteCommand.commandName, remoteCommand});
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

const RemoteCommand& IRemoteServer::getRemoteCommand(const QString& commandName) const
{
    return m_remoteCommands.at(commandName);
}
