/*
Copyright 2017-2018 Milovidov Mikhail

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "Precompiled.h"
#include "CSourceServerSshConnection.h"

#include <QSshLib/sshconnection.h>

#include "CSourceServerAuthorization.h"

using namespace QSsh;

CSourceServerSshConnection::CSourceServerSshConnection(const QString& serverId,
                                                       const CSourceServerAuthorization& sourceServersAuthorization,
                                                       const QMap<QString, CSourceServerCommand>& commands,
                                                       const int connectionTimeout,
                                                       QObject* pParent)
    : QObject(pParent)
    , m_serverId(serverId)
    , m_connectionTimeout(connectionTimeout)
    , m_pSshConnection(new SshConnection(convertToSshConnectionParameters(sourceServersAuthorization)))
    , m_sourceServerCommands(commands)
{
    setObjectName(serverId);

    connect(m_pSshConnection, &SshConnection::connected, this, &CSourceServerSshConnection::onHostConnected);
    connect(m_pSshConnection, &SshConnection::disconnected, this, &CSourceServerSshConnection::onHostDisconnected);
    connect(m_pSshConnection, &SshConnection::error, this, &CSourceServerSshConnection::onHostError);
}

CSourceServerSshConnection::~CSourceServerSshConnection()
{
    killConnection();
    m_pSshConnection->deleteLater();
}

void CSourceServerSshConnection::connectToHost()
{
    m_pSshConnection->connectToHost();
}

bool CSourceServerSshConnection::isConnected() const
{
    return m_pSshConnection->state() == SshConnection::Connected;
}

const QString& CSourceServerSshConnection::serverId() const
{
    return m_serverId;
}

void CSourceServerSshConnection::killConnection()
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
        connect(m_pKillChildsProcess.data(), &SshRemoteProcess::closed, this, &CSourceServerSshConnection::closeConnection);
        m_pKillChildsProcess->start();
    }
}


void CSourceServerSshConnection::onHostConnected()
{
    for (const QString& commandId : m_sourceServerCommands.keys())
        startRemoteProcess(commandId);
    emit connectedToHost(m_serverId);
}

void CSourceServerSshConnection::onHostDisconnected()
{
    emit disconnectedFromHost(m_serverId);
}

void CSourceServerSshConnection::onHostError()
{
    emit hostError(m_serverId, m_pSshConnection->errorString());
}

void CSourceServerSshConnection::onCommandStarted()
{
    const QString& commandId = sender()->objectName();
    emit commandStarted(m_serverId, commandId, m_sourceServerCommands[commandId].outputExtension());
}

void CSourceServerSshConnection::onNewStandardStreamData()
{
    const QString& commandId = sender()->objectName();
    QSharedPointer<SshRemoteProcess> remoteProcess = m_remoteProcesses[commandId];
    emit newStandardStreamData(m_serverId, commandId, remoteProcess->readAllStandardOutput());
}

void CSourceServerSshConnection::onNewErrorStreamData()
{
    const QString& commandId = sender()->objectName();
    QSharedPointer<SshRemoteProcess> remoteProcess = m_remoteProcesses[commandId];
    emit newErrorStreamData(m_serverId, commandId, remoteProcess->readAllStandardError());
}

void CSourceServerSshConnection::onCommandWasExit(int exitStatus)
{
    const QString& commandId = sender()->objectName();
    const QSharedPointer<QSsh::SshRemoteProcess>& pSshRemoteProcess = m_remoteProcesses[commandId];
    emit commandWasExit(m_serverId, commandId, (DataSourcesRemoteProcessExitStatus)exitStatus, pSshRemoteProcess->exitCode());
    m_remoteProcesses.remove(commandId);
    if (m_remoteProcesses.isEmpty())
        closeConnection();
}

void CSourceServerSshConnection::closeConnection()
{
    m_pSshConnection->closeAllChannels();
    m_remoteProcesses.clear();
    m_pSshConnection->disconnectFromHost();
}

void CSourceServerSshConnection::startRemoteProcess(const QString& commandId)
{
    const CSourceServerCommand& remoteCommand = m_sourceServerCommands[commandId];

    QSharedPointer<SshRemoteProcess> remoteProcess = m_pSshConnection->createRemoteProcess(qPrintable(remoteCommand.command()));
    remoteProcess->setObjectName(commandId);

    connect(remoteProcess.data(), &SshRemoteProcess::started, this, &CSourceServerSshConnection::onCommandStarted);
    connect(remoteProcess.data(), &SshRemoteProcess::readyReadStandardOutput, this, &CSourceServerSshConnection::onNewStandardStreamData);
    connect(remoteProcess.data(), &SshRemoteProcess::readyReadStandardError, this, &CSourceServerSshConnection::onNewErrorStreamData);
    connect(remoteProcess.data(), &SshRemoteProcess::closed, this, &CSourceServerSshConnection::onCommandWasExit);

    m_remoteProcesses[commandId] = remoteProcess;
    remoteProcess->start();
}


SshConnectionParameters CSourceServerSshConnection::convertToSshConnectionParameters(const CSourceServerAuthorization& sourceServersAuthorization) const
{
    SshConnectionParameters result;
    result.host = sourceServersAuthorization.host();
    result.userName = sourceServersAuthorization.login();
    if (sourceServersAuthorization.authorizationByKey()) {
        result.authenticationType = SshConnectionParameters::AuthenticationTypePublicKey;
        result.privateKeyFile = sourceServersAuthorization.keyFile();
    } else {
        result.authenticationType = SshConnectionParameters::AuthenticationTypePassword;
        result.password = sourceServersAuthorization.password();
    }
    result.port = 22;
    result.timeout = m_connectionTimeout;
    result.options = SshConnectionOption::SshIgnoreDefaultProxy;
    return result;
}
