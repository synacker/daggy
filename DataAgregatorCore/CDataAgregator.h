/*
Copyright 2017-2018 Milovidov Mikhail

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef CDATAAGREGATOR_H
#define CDATAAGREGATOR_H

#include "dataagregatorcore_global.h"

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QMap>

#include "IDataSourcesSender.h"

namespace QSsh {
class SshConnection;
}

#include "CSourceServerSshConnection.h"

class CSourceServerSshConnection;
class CSourceServerAuthorization;
class CDataSources;
class IDataSourcesReciever;

class DATAAGREGATORCORESHARED_EXPORT CDataAgregator : public QObject, public IDataSourcesSender
{
    Q_OBJECT
public:
    explicit CDataAgregator(const bool isReconnectionMode, const int connectionTimeout, QObject *pParent = nullptr);
    ~CDataAgregator();

    void start(const CDataSources& dataSources);
    void stop();

    bool isStarted() const;

    void addDataSourcesReciever(IDataSourcesReciever* pDataSourcesReciever);
    void removeDataSourcesReciever(IDataSourcesReciever* pDataSourcesReciever);

signals:
    void connectedToHost(QString serverId) override;
    void disconnectedFromHost(QString serverId) override;
    void hostError(QString serverId, QString errorString) override;

    void newStandardStreamData(QString serverId, QString commandId, QByteArray data) override;
    void newErrorStreamData(QString serverId, QString commandId, QByteArray data) override;

    void commandStarted(QString serverId, QString commandId, QString outputExtension) override;
    void commandWasExit(QString serverId, QString commandId, DataSourcesRemoteProcessExitStatus exitStatus, int exitCode) override;

    void stopped();

private slots:
    void onServerDisconnected(QString serverId);

private:
    int getActiveConnectionsCount() const;
    void closeConnections();


    const bool m_isRecconnectionMode;
    const int m_connectionTimeout;
    bool m_isReconnectSshConnections;
    QMap<QString, CSourceServerSshConnection*> m_sshConnections;
};

#endif // CDATAAGREGATOR_H
