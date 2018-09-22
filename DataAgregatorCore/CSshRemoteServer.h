#ifndef CSSHREMOTESERVER_H
#define CSSHREMOTESERVER_H

#include <QObject>
#include <QSharedPointer>
#include <QVariantMap>

#include "IRemoteServer.h"

namespace QSsh {
    class SshConnectionParameters;
    class SshConnection;
    class SshRemoteProcess;
};


class CSshRemoteServer : public IRemoteServer
{
    Q_OBJECT
public:
    CSshRemoteServer(const QString& serverName,
                     const QVector<RemoteCommand>& commands,
                     const QVariantMap& connectionParameters,
                     QObject* pParent = nullptr);
    ~CSshRemoteServer() override;

    // IRemoteServer interface
    void connectToServer() override;
    void startCommand(const QString& commandName) override;
    void stop() override;


private slots:
    void onHostConnected();
    void onHostDisconnected();
    void onHostError();

    void onNewStandardStreamData();
    void onNewErrorStreamData();

    void onCommandStarted();
    void onCommandWasExit(int exitStatus);
private:
    void killConnection();
    void closeConnection();

    QSsh::SshConnection* const m_pSshConnection;
    const bool m_isForceKill;
    QSharedPointer<QSsh::SshRemoteProcess> m_pKillChildsProcess;
};

#endif // CSSHREMOTESERVER_H
