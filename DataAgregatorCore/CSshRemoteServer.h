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
    CSshRemoteServer(const DataSource& data_source,
                     QObject* parent_pointer = nullptr);
    ~CSshRemoteServer() override;

    bool isForceKill() const;

    static constexpr const char* connection_type_global = "ssh";

private slots:
    void onHostConnected();
    void onHostDisconnected();
    void onHostError();

    void onNewStandardStreamData();
    void onNewErrorStreamData();

    void onCommandStarted();
    void onCommandWasExit(int exitStatus);

private:
    // IRemoteServer interface
    void startAgregator() override final;
    void stopAgregator() override final;
    void reconnect() override final;

    void restartCommand(const QString& command_name) override final;

    void killConnection();
    void closeConnection();

    void startRemoteSshProcess(const QString& command_name, const QString& command);
    QSharedPointer<QSsh::SshRemoteProcess> getSshRemoteProcess(const QString& command_name) const;

    QSsh::SshConnection* const ssh_connection_pointer_;
    const int force_kill_;

    QMap<QString, QSharedPointer<QSsh::SshRemoteProcess>> ssh_processes_;
    QSharedPointer<QSsh::SshRemoteProcess> kill_childs_process_pointer_ = nullptr;
};

#endif // CSSHREMOTESERVER_H
