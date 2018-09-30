#include "Precompiled.h"
#include "IRemoteServer.h"

#include "DataSource.h"

IRemoteServer::IRemoteServer(const DataSource& data_source,
                             QObject* parent_ptr)
    : IRemoteAgregator(parent_ptr)
    , data_source_(data_source)
    , remote_commands_(convertRemoteCommands(data_source.remote_commands))
    , exists_restart_commands_(isExistsRestartCommand(data_source.remote_commands))
{
    setObjectName(data_source.server_name);
    for (const auto& pair : remote_commands_) {
        commands_status_[pair.first] = RemoteCommand::Status::NotStarted;
    }
}

IRemoteServer::~IRemoteServer()
{
}

const QString& IRemoteServer::serverName() const
{
    return data_source_.server_name;
}

void IRemoteServer::startCommands()
{
    for (const auto& pair : remote_commands_) {
        const QString& commandName = pair.first;
        if (commandStatus(commandName) != RemoteCommand::Status::Started)
            restartCommand(commandName);
    }
}

bool IRemoteServer::isExistsRestartCommand(const std::vector<RemoteCommand>& commands) const
{
    bool result = false;
    size_t index = 0;
    while (!result && index < commands.size()) {
        result = commands.at(index).restart;
        index++;
    }
    return result;
}

void IRemoteServer::setConnectionStatus(const RemoteConnectionStatus status, const QString& message)
{
    if (connection_status_ != status) {
        connection_status_ = status;
        emit connectionStatusChanged(data_source_.server_name, status, message);
        if (status != RemoteConnectionStatus::Connected) {
            if (data_source_.reconnect && state() == State::Run)
                reconnect();
            else {
                setStopped();
            }
        } else {
            startCommands();
        }
    }
}

void IRemoteServer::setRemoteCommandStatus(const QString& command_name,
                                           const RemoteCommand::Status command_status,
                                           const int exit_code)
{
    const RemoteCommand::Status current_status = commands_status_[command_name];
    if (current_status != command_status) {
        commands_status_[command_name] = command_status;
        const RemoteCommand& remote_command = getRemoteCommand(command_name);
        emit remoteCommandStatusChanged(data_source_.server_name,
                                        remote_command,
                                        command_status,
                                        exit_code);
        if (state() == State::Run &&
            command_status != RemoteCommand::Status::Started)
        {
            if (remote_command.restart)
                restartCommand(command_name);
            else if (!isExistsRestartCommand() && !isExistsRunningRemoteCommands())
                stopAgregator();
        }
    }
}

void IRemoteServer::setNewRemoteCommandStream(const QString& commandName, const QByteArray& data, const RemoteCommand::Stream::Type type)
{
    const RemoteCommand& pRemoteCommand = getRemoteCommand(commandName);
    emit newRemoteCommandStream(data_source_.server_name, {commandName, pRemoteCommand.output_extension, data, type});
}

std::map<QString, RemoteCommand> IRemoteServer::convertRemoteCommands(const std::vector<RemoteCommand>& remoteCommands) const
{
    std::map<QString, RemoteCommand> result;

    for (const RemoteCommand& remoteCommand : remoteCommands) {
        result.insert({remoteCommand.command_name, remoteCommand});
    }

    return result;
}

const QString& IRemoteServer::connectionType() const
{
    return data_source_.connection_type;
}

const QVariantMap&IRemoteServer::connectionParameters() const
{
    return data_source_.connection_parameters;
}

const DataSource&IRemoteServer::dataSource() const
{
    return data_source_;
}

RemoteConnectionStatus IRemoteServer::connectionStatus() const
{
    return connection_status_;
}

RemoteCommand::Status IRemoteServer::commandStatus(const QString& commandName) const
{
    return commands_status_.value(commandName, RemoteCommand::Status::NotStarted);
}

const RemoteCommand& IRemoteServer::getRemoteCommand(const QString& commandName) const
{
    return remote_commands_.at(commandName);
}

bool IRemoteServer::isExistsRestartCommand() const
{
    return exists_restart_commands_;
}

size_t IRemoteServer::runingRemoteCommandsCount() const
{
    size_t result = 0;
    for (const QString& command_name : commands_status_.keys()) {
        if (commands_status_[command_name] == RemoteCommand::Status::Started)
            result++;
    }
    return result;
}
