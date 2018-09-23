#include "Precompiled.h"
#include "CSshRemoteServer.h"

#include <ssh/sshconnection.h>
#include <ssh/sshremoteprocess.h>

using namespace QSsh;

namespace {

const QString g_hostField("host");
const QString g_loginField("login");
const QString g_passwordField("password");
const QString g_keyField("keyFile");
const QString g_forceKill("forceKill");
}

SshConnectionParameters getConnectionParameters(const QVariantMap& authorizationParameters);
RemoteCommand::Status convertStatus(const SshRemoteProcess::ExitStatus exitStatus);

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

void CSshRemoteServer::startRemoteSshProcess(const QString& commandName, const QString& command)
{
    QSharedPointer<SshRemoteProcess> remoteProcess = m_pSshConnection->createRemoteProcess(qPrintable(command));
    remoteProcess->setObjectName(commandName);
    remoteProcess->setParent(this);

    connect(remoteProcess.data(), &SshRemoteProcess::started, this, &CSshRemoteServer::onCommandStarted);
    connect(remoteProcess.data(), &SshRemoteProcess::readyReadStandardOutput, this, &CSshRemoteServer::onNewStandardStreamData);
    connect(remoteProcess.data(), &SshRemoteProcess::readyReadStandardError, this, &CSshRemoteServer::onNewErrorStreamData);
    connect(remoteProcess.data(), &SshRemoteProcess::closed, this, &CSshRemoteServer::onCommandWasExit);

    remoteProcess->start();
}

void CSshRemoteServer::startCommand(const QString& commandName)
{
    const RemoteCommand* const pRemoteCommand = getRemoteCommand(commandName);

    if (pRemoteCommand) {
        startRemoteSshProcess(commandName, pRemoteCommand->command);
    }
}

void CSshRemoteServer::stop()
{
    if (m_isForceKill)
        killConnection();
    else
        closeConnection();
}

size_t CSshRemoteServer::runingRemoteCommandsCount() const
{
    size_t result = 0;

    for (SshRemoteProcess* pRemoteProcess : sshRemoteProcesses()) {
        if (pRemoteProcess->isRunning())
            result++;
    }

    return result;
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

void CSshRemoteServer::onNewStandardStreamData()
{
    const QString& commandName = sender()->objectName();
    SshRemoteProcess* const pSshRemoteProcess = getSshRemoteProcess(commandName);
    if (pSshRemoteProcess) {
        setNewRemoteCommandStream(commandName, pSshRemoteProcess->readAllStandardOutput(), RemoteCommand::Stream::Type::Standard);
    }
}

void CSshRemoteServer::onNewErrorStreamData()
{
    const QString& commandName = sender()->objectName();
    SshRemoteProcess* const pSshRemoteProcess = getSshRemoteProcess(commandName);
    if (pSshRemoteProcess) {
        setNewRemoteCommandStream(commandName, pSshRemoteProcess->readAllStandardError(), RemoteCommand::Stream::Type::Error);
    }
}

void CSshRemoteServer::onCommandStarted()
{
    const QString& commandName = sender()->objectName();
    setRemoteCommandStatus(commandName, RemoteCommand::Status::Started);
}

void CSshRemoteServer::onCommandWasExit(int exitStatus)
{
    const QString& commandName = sender()->objectName();
    SshRemoteProcess* const pSshRemoteProcess = getSshRemoteProcess(commandName);
    if (pSshRemoteProcess) {
        const SshRemoteProcess::ExitStatus status = static_cast<SshRemoteProcess::ExitStatus>(exitStatus);
        setRemoteCommandStatus(commandName, convertStatus(status), pSshRemoteProcess->exitCode());
    }

    if (!isExistsRunningRemoteCommands())
        m_pSshConnection->disconnectFromHost();
}

void CSshRemoteServer::killConnection()
{
    if (m_pSshConnection->state() == SshConnection::Connected &&
       (!m_pKillChildsProcess || !m_pKillChildsProcess->isRunning()))
    {
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
        for (SshRemoteProcess* pRemoteProcess : sshRemoteProcesses()) {
            if (pRemoteProcess->isRunning())
                pRemoteProcess->close();
        }

        if (!isExistsRunningRemoteCommands())
            m_pSshConnection->disconnectFromHost();
    }
}

QList<SshRemoteProcess*> CSshRemoteServer::sshRemoteProcesses() const
{
    return findChildren<SshRemoteProcess*>();
}

SshRemoteProcess* CSshRemoteServer::getSshRemoteProcess(const QString& commandName) const
{
    return findChild<SshRemoteProcess*>(commandName);
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

RemoteCommand::Status convertStatus(const SshRemoteProcess::ExitStatus exitStatus)
{
    RemoteCommand::Status remoteCommandStatus;
    switch (exitStatus) {
    case SshRemoteProcess::FailedToStart:
        remoteCommandStatus = RemoteCommand::Status::FailedToStart;
        break;
    case SshRemoteProcess::CrashExit:
        remoteCommandStatus = RemoteCommand::Status::CrashExit;
        break;
    case SshRemoteProcess::NormalExit:
        remoteCommandStatus = RemoteCommand::Status::NormalExit;
        break;
    }

    return remoteCommandStatus;
}
