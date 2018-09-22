#ifndef CSSHREMOTESERVER_H
#define CSSHREMOTESERVER_H

#include <QObject>
#include <QSharedPointer>
#include <QVariantMap>

#include "IRemoteServer.h"

namespace QSsh {
    class SshConnectionParameters;
    class SshConnection;
};


class CSshRemoteServer : public IRemoteServer
{
    Q_OBJECT
public:
    CSshRemoteServer(const QString& serverName,
                     const QVector<RemoteCommand>& commands,
                     const QVariantMap& authorizationParameters,
                     QObject* pParent = nullptr);
    virtual ~CSshRemoteServer() override = default;
    // IRemoteServer interface
public:
    void connect() override;
    void startCommand(const QString& commandName) override;

private slots:
    void onHostConnected();
    void onHostDisconnected();
    void onHostError();

private:
    QSsh::SshConnection* const m_pSshConnection;
};

#endif // CSSHREMOTESERVER_H
