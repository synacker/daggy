#include "Precompiled.h"
#include "CSshRemoteServer.h"

#include <ssh/sshconnection.h>
#include <ssh/sshremoteprocess.h>

using namespace QSsh;

namespace  {
constexpr const char* g_connectionType = "ssh";

const QString g_hostField("host");
const QString g_loginField("login");
const QString g_passwordField("password");
const QString g_keyField("keyFile");
const QString g_forceKill("forceKill");

}

QSsh::SshConnectionParameters getConnectionParameters(const QVariantMap& authorizationParameters);

CSshRemoteServer::CSshRemoteServer(const QString& serverName,
                                   const QVector<RemoteCommand>& commands,
                                   const QVariantMap& connectionParameters,
                                   QObject* pParent)
    : IRemoteServer(serverName, g_connectionType, commands, pParent)
    , m_pSshConnection(new SshConnection(getConnectionParameters(connectionParameters), this))
    , m_isForceKill(connectionParameters.value(g_forceKill, false).toBool())
{
    connect(m_pSshConnection, &SshConnection::connected, this, &CSshRemoteServer::onHostConnected);
    connect(m_pSshConnection, &SshConnection::disconnected, this, &CSshRemoteServer::onHostDisconnected);
    connect(m_pSshConnection, &SshConnection::error, this, &CSshRemoteServer::onHostError);
}

CSshRemoteServer::~CSshRemoteServer()
{
    closeConnection();
}

void CSshRemoteServer::connectToServer()
{
    m_pSshConnection->connectToHost();
}

void CSshRemoteServer::startCommand(const QString& commandName)
{
    const RemoteCommand& remoteCommand = getRemoteCommand(commandName);

    QSharedPointer<SshRemoteProcess> remoteProcess = m_pSshConnection->createRemoteProcess(qPrintable(remoteCommand.command));
    remoteProcess->setObjectName(commandName);
    remoteProcess->setParent(this);

    connect(remoteProcess.data(), &SshRemoteProcess::started, this, &CSshRemoteServer::onCommandStarted);
    connect(remoteProcess.data(), &SshRemoteProcess::readyReadStandardOutput, this, &CSshRemoteServer::onNewStandardStreamData);
    connect(remoteProcess.data(), &SshRemoteProcess::readyReadStandardError, this, &CSshRemoteServer::onNewErrorStreamData);
    connect(remoteProcess.data(), &SshRemoteProcess::closed, this, &CSshRemoteServer::onCommandWasExit);


}

void CSshRemoteServer::stop()
{
    if (m_isForceKill)
        killConnection();
    else
        closeConnection();
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

void CSshRemoteServer::killConnection()
{
    if (m_pSshConnection->state() == SshConnection::Connected) {
        m_pKillChildsProcess = m_pSshConnection->createRemoteProcess("pids=$(pstree -p $PPID | grep -oP \"\\d+\" | grep -v $PPID | grep -v $$ | tac);"
                                                                     "for pid in $pids; do "
                                                                        "while kill -0 $pid; do "
                                                                            "kill -15 $pid;"
                                                                            "sleep 0.1;"
                                                                         "done "
                                                                     "done "
                                                                    );
        connect(m_pKillChildsProcess.data(), &SshRemoteProcess::closed, this, &CSshRemoteServer::closeConnection);
        m_pKillChildsProcess->start();
    }
}

void CSshRemoteServer::closeConnection()
{
    if (m_pSshConnection->state() == SshConnection::Connected) {
        m_pSshConnection->closeAllChannels();
        //m_remoteProcesses.clear();
        m_pSshConnection->disconnectFromHost();
    }
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
