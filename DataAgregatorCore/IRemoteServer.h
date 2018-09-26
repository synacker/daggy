#ifndef ISERVERCONNECTION_H
#define ISERVERCONNECTION_H

#include "dataagregatorcore_global.h"

#include <QObject>
#include <QString>
#include <QVector>
#include <QMap>

#include "IRemoteAgregator.h"

class DATAAGREGATORCORESHARED_EXPORT IRemoteServer : public IRemoteAgregator
{
    Q_OBJECT
public:

    IRemoteServer(const QString& serverName,
                  const QString& connectionType,
                  const std::vector<RemoteCommand>& commands,
                  QObject* pParent = nullptr);
    ~IRemoteServer() override;

    QString serverName() const;
    QString connectionType() const;
    ConnectionStatus connectionStatus() const;
    RemoteCommand::Status commandStatus(const QString& commandName) const;
    const RemoteCommand& getRemoteCommand(const QString& commandName) const;

protected:
    virtual void restartCommand(const QString& commandName) = 0;

    void setConnectionStatus(const ConnectionStatus status, const QString& message = QString());
    void setRemoteCommandStatus(const QString& commandName, RemoteCommand::Status commandStatus, const int exitCode = 0);
    void setNewRemoteCommandStream(const QString& commandName, const QByteArray& data, const RemoteCommand::Stream::Type type);

private:
    void startCommands();

    std::map<QString, RemoteCommand> convertRemoteCommands(const std::vector<RemoteCommand>& remoteCommands) const;

    const QString m_serverName;
    const QString m_connectionType;
    const std::map<QString, RemoteCommand> m_remoteCommands;
    QMap<QString, RemoteCommand::Status> m_commandsStatus;

    ConnectionStatus m_connectionStatus = ConnectionStatus::NotConnected;
};

#endif // ISERVERCONNECTION_H
