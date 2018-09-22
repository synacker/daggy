#include "Precompiled.h"
#include "CSshRemoteServer.h"

#include <ssh/sshconnection.h>

using namespace QSsh;

namespace  {
constexpr const char* g_connectionType = "ssh";

const QString g_hostField("host");
const QString g_loginField("login");
const QString g_passwordField("password");
const QString g_keyField("keyFile");

}

QSsh::SshConnectionParameters getConnectionParameters(const QVariantMap& authorizationParameters);

CSshRemoteServer::CSshRemoteServer(const QString& serverName,
                                   const QVector<RemoteCommand>& commands,
                                   const QVariantMap& authorizationParameters,
                                   QObject* pParent)
    : IRemoteServer(serverName, g_connectionType, commands, pParent)
    , m_pSshConnection(new SshConnection(getConnectionParameters(authorizationParameters), this))
{
    connect(m_pSshConnection, &SshConnection::connected, this, &CSshRemoteServer::onHostConnected);
    connect(m_pSshConnection, &SshConnection::disconnected, this, &CSshRemoteServer::onHostDisconnected);
    connect(m_pSshConnection, &SshConnection::error, this, &CSshRemoteServer::onHostError);
}

void CSshRemoteServer::connectToServer()
{
    m_pSshConnection->connectToHost();
}

void CSshRemoteServer::onHostConnected()
{
    setConnectionStatus(ConnectionStatus::Connected);
}

void CSshRemoteServer::onHostDisconnected()
{
    setConnectionStatus(ConnectionStatus::Disconnected);
}

void CSshRemoteServer::onHostError()
{
    setConnectionStatus(ConnectionStatus::ConnectionError, m_pSshConnection->errorString());
}

QSsh::SshConnectionParameters getConnectionParameters(const QVariantMap& authorizationParameters)
{
    SshConnectionParameters result;

    const QString& host = authorizationParameters[g_hostField].toString();
    const QString& login = authorizationParameters[g_loginField].toString();
    const QString& password = authorizationParameters[g_passwordField].toString();
    const QString& keyFile = authorizationParameters[g_keyField].toString();

    result.setHost(host);
    result.setUserName(login);
    result.setPassword(password);
    result.privateKeyFile = keyFile;
    result.authenticationType = keyFile.isEmpty() ? SshConnectionParameters::AuthenticationTypePassword : SshConnectionParameters::AuthenticationTypePublicKey;

    return result;
}
