#ifndef ISERVERCONNECTION_H
#define ISERVERCONNECTION_H

#include "dataagregatorcore_global.h"

#include <QObject>
#include <QString>
#include <QVector>
#include <QMap>

#include "RemoteCommand.h"

class DATAAGREGATORCORESHARED_EXPORT IRemoteServer : public QObject
{
    Q_OBJECT
public:
    enum class ConnectionStatus {
        NotConnected,
        Connected,
        Disconnected,
        ConnectionError
    };

    IRemoteServer(const QString& serverName,
                  const QString& connectionType,
                  const QVector<RemoteCommand>& commands,
                  QObject* pParent = nullptr);
    virtual ~IRemoteServer();

    QString serverName() const;
    QString connectionType() const;
    ConnectionStatus connectionStatus() const;
    RemoteCommand::Status commandStatus(const QString& commandName) const;
    const RemoteCommand* getRemoteCommand(const QString& commandName) const;
    bool isExistsRunningRemoteCommands() const;

    void startCommands();

    virtual void connectToServer() = 0;
    virtual void startCommand(const QString& commandName) = 0;
    virtual void stop() = 0;
    virtual size_t runingRemoteCommandsCount() const = 0;

signals:
    void connectionStatusChanged(QString serverId, ConnectionStatus status, QString message);
    void remoteCommandStatusChanged(QString serverId, QString commandName, RemoteCommand::Status status, int exitCode);
    void newRemoteCommandStream(QString serverId, RemoteCommand::Stream stream);

protected:
    void setConnectionStatus(const ConnectionStatus status, const QString& message = QString());
    void setRemoteCommandStatus(QString commandName, RemoteCommand::Status status, int exitCode = 0);
    void setNewRemoteCommandStream(const QString& commandName, const QByteArray& data, const RemoteCommand::Stream::Type type);

private:
    QMap<QString, RemoteCommand*> convertRemoteCommands(const QVector<RemoteCommand>& remoteCommands) const;

    const QString m_serverName;
    const QString m_connectionType;
    const QMap<QString, RemoteCommand*> m_remoteCommands;
    QMap<QString, RemoteCommand::Status> m_commandsStatus;

    ConnectionStatus m_connectionStatus = ConnectionStatus::NotConnected;
};

#endif // ISERVERCONNECTION_H
