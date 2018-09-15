/*
Copyright 2017-2018 Milovidov Mikhail

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "Precompiled.h"
#include "CDataAgregator.h"

#include <QSshLib/sshconnection.h>


#include "CDataSources.h"
#include "CSourceServer.h"
#include "CSourceServerSshConnection.h"
#include "IDataSourcesReciever.h"

using namespace QSsh;



CDataAgregator::CDataAgregator(const bool isReconnectionMode, const int connectionTimeout, QObject* pParent)
    : QObject(pParent)
    , m_isRecconnectionMode(isReconnectionMode)
    , m_connectionTimeout(connectionTimeout)
{
    connect(this, &CDataAgregator::disconnectedFromHost, this, &CDataAgregator::onServerDisconnected);
}

CDataAgregator::~CDataAgregator()
{
    qDeleteAll(m_sshConnections);
}

void CDataAgregator::start(const CDataSources& dataSources)
{
    if (!isStarted()) {
        m_isReconnectSshConnections = m_isRecconnectionMode;

        for (const QString& sourceServerId : dataSources.sourceServers()) {
            CSourceServer* pSourceServer = dataSources.sourceServer(sourceServerId);

            CSourceServerSshConnection* pSourceServerSshConnection = new CSourceServerSshConnection(sourceServerId,
                                                                                                    pSourceServer->sourceServerAuthorization(),
                                                                                                    pSourceServer->sourceServerCommands(),
                                                                                                    m_connectionTimeout,
                                                                                                    this);
            pSourceServerSshConnection->setObjectName(sourceServerId);

            connect(pSourceServerSshConnection, &CSourceServerSshConnection::connectedToHost, this, &CDataAgregator::connectedToHost);
            connect(pSourceServerSshConnection, &CSourceServerSshConnection::disconnectedFromHost, this, &CDataAgregator::disconnectedFromHost);
            connect(pSourceServerSshConnection, &CSourceServerSshConnection::hostError, this, &CDataAgregator::hostError);

            connect(pSourceServerSshConnection, &CSourceServerSshConnection::newStandardStreamData, this, &CDataAgregator::newStandardStreamData);
            connect(pSourceServerSshConnection, &CSourceServerSshConnection::newErrorStreamData, this, &CDataAgregator::newErrorStreamData);

            connect(pSourceServerSshConnection, &CSourceServerSshConnection::commandStarted, this, &CDataAgregator::commandStarted);
            connect(pSourceServerSshConnection, &CSourceServerSshConnection::commandWasExit, this, &CDataAgregator::commandWasExit);

            pSourceServerSshConnection->connectToHost();

            m_sshConnections[sourceServerId] = pSourceServerSshConnection;
        }
    }
}

void CDataAgregator::stop()
{
    m_isReconnectSshConnections = false;
    if (getActiveConnectionsCount() == 0)
        emit stopped();
    else
        closeConnections();
}

bool CDataAgregator::isStarted() const
{
    return !m_sshConnections.isEmpty();
}

void CDataAgregator::addDataSourcesReciever(IDataSourcesReciever* pDataSourcesReciever)
{
    connect(this, &CDataAgregator::connectedToHost, pDataSourcesReciever, &IDataSourcesReciever::onConnectedToHost);
    connect(this, &CDataAgregator::disconnectedFromHost, pDataSourcesReciever, &IDataSourcesReciever::onDisconnectedFromHost);
    connect(this, &CDataAgregator::hostError, pDataSourcesReciever, &IDataSourcesReciever::onHostError);

    connect(this, &CDataAgregator::newStandardStreamData, pDataSourcesReciever, &IDataSourcesReciever::onNewStandardStreamData);
    connect(this, &CDataAgregator::newErrorStreamData, pDataSourcesReciever, &IDataSourcesReciever::onNewErrorStreamData);

    connect(this, &CDataAgregator::commandStarted, pDataSourcesReciever, &IDataSourcesReciever::onCommandStarted);
    connect(this, &CDataAgregator::commandWasExit, pDataSourcesReciever, &IDataSourcesReciever::onCommandWasExit);
}

void CDataAgregator::removeDataSourcesReciever(IDataSourcesReciever* pDataSourcesReciever)
{
    disconnect(pDataSourcesReciever);
}

void CDataAgregator::onServerDisconnected(QString serverId)
{
    CSourceServerSshConnection* pSourceServerSshConnection = m_sshConnections[serverId];
    if (m_isReconnectSshConnections)
        pSourceServerSshConnection->connectToHost();
    else {
        delete pSourceServerSshConnection;
        m_sshConnections.remove(serverId);
    }

    if (getActiveConnectionsCount() == 0)
        emit stopped();
}

int CDataAgregator::getActiveConnectionsCount() const
{
    int result = 0;
    for (CSourceServerSshConnection* pSshConnection : m_sshConnections.values())
        if (pSshConnection->isConnected())
            result++;
    return result;
}

void CDataAgregator::closeConnections()
{
    for (CSourceServerSshConnection* pSshConnection : m_sshConnections.values()) {
        pSshConnection->killConnection();
    }
}
