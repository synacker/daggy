#ifndef ISERVERCONNECTION_H
#define ISERVERCONNECTION_H

#include "dataagregatorcore_global.h"

#include <QObject>
#include <QString>
#include <QVector>
#include <QMap>

class DATAAGREGATORCORESHARED_EXPORT IRemoteServer : public QObject
{
    Q_OBJECT
public:
    struct RemoteCommand {
        enum class Status {
            NotStarted,
            Started,
            FailedToStart,
            CrashExit,
            NormalExit
        };

        struct Stream {
            enum class Type {
                Standard,
                Error
            };

            const QString commandName;
            const QString outputExtension;
            const QByteArray data;
            const Type type;
        };

        const QString commandName;
        const QString command;
        const QString outputExtension;
    };


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
    virtual ~IRemoteServer() = default;

    QString serverName() const;
    QString connectionType() const;
    ConnectionStatus connectionStatus() const;
    RemoteCommand::Status commandStatus(const QString& commandName) const;
    RemoteCommand remoteCommand(const QString& commandName) const;

    void startCommands();

    virtual void connect() = 0;
    virtual void startCommand(const QString& commandName) = 0;

signals:
    void connectionStatusChanged(QString serverId, ConnectionStatus status, QString message);
    void remoteCommandStatusChanged(QString serverId, QString commandName, RemoteCommand::Status status, int statusCode);
    void newRemoteCommandStream(QString serverId, RemoteCommand::Stream stream);

protected:
    void setConnectionStatus(const ConnectionStatus status, const QString& message = QString());
    void setRemoteCommandStatus(QString commandName, RemoteCommand::Status status, int statusCode = 0);
    void setNewRemoteCommandStream(const QString& commandName, const QByteArray& data, const RemoteCommand::Stream::Type type);

private:
    QMap<QString, RemoteCommand> convert(const QVector<RemoteCommand>& remoteCommands) const;

    const QString m_serverName;
    const QString m_connectionType;
    const QMap<QString, RemoteCommand> m_remoteCommands;
    QMap<QString, RemoteCommand::Status> m_commandsStatus;

    ConnectionStatus m_connectionStatus = ConnectionStatus::NotConnected;
};

#endif // ISERVERCONNECTION_H
