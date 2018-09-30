#ifndef ISERVERCONNECTION_H
#define ISERVERCONNECTION_H

#include "dataagregatorcore_global.h"

#include <QObject>
#include <QString>
#include <QVector>
#include <QMap>

#include "IRemoteAgregator.h"
#include "DataSource.h"

class DATAAGREGATORCORESHARED_EXPORT IRemoteServer : public IRemoteAgregator
{
    Q_OBJECT
public:
    IRemoteServer(const DataSource& data_source,
                  QObject* parent_ptr = nullptr);
    ~IRemoteServer() override;

    const QString& serverName() const;
    const QString& connectionType() const;
    const QVariantMap& connectionParameters() const;

    const DataSource& dataSource() const;

    RemoteConnectionStatus connectionStatus() const;
    RemoteCommand::Status commandStatus(const QString& commandName) const;
    const RemoteCommand& getRemoteCommand(const QString& commandName) const;

    bool isExistsRestartCommand() const;

    size_t runingRemoteCommandsCount() const override final;

protected:
    virtual void restartCommand(const QString& commandName) = 0;
    virtual void reconnect() = 0;

    void setConnectionStatus(const RemoteConnectionStatus status, const QString& message = QString());
    void setRemoteCommandStatus(const QString& commandName, const RemoteCommand::Status commandStatus, const int exit_code = 0);
    void setNewRemoteCommandStream(const QString& commandName, const QByteArray& data, const RemoteCommand::Stream::Type type);

private:
    void startCommands();
    bool isExistsRestartCommand(const std::vector<RemoteCommand>& commands) const;
    std::map<QString, RemoteCommand> convertRemoteCommands(const std::vector<RemoteCommand>& remoteCommands) const;

    const DataSource data_source_;
    const std::map<QString, RemoteCommand> remote_commands_;
    const bool exists_restart_commands_;
    QMap<QString, RemoteCommand::Status> commands_status_;

    RemoteConnectionStatus connection_status_ = RemoteConnectionStatus::NotConnected;
};

#endif // ISERVERCONNECTION_H
