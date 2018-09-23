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
    size_t runingRemoteCommandsCount() const override;

    static constexpr const char* g_connectionType = "ssh";

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

    QList<QSsh::SshRemoteProcess*> sshRemoteProcesses() const;
    QSsh::SshRemoteProcess* getSshRemoteProcess(const QString& commandName) const;

    QSsh::SshConnection* const m_pSshConnection;
    const bool m_isForceKill;
    QSharedPointer<QSsh::SshRemoteProcess> m_pKillChildsProcess = nullptr;
    void startRemoteSshProcess(const QString& commandName, const QString& command);
};

#endif // CSSHREMOTESERVER_H
