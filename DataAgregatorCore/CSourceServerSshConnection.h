/*
Copyright 2017-2018 Milovidov Mikhail

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef CSOURCESERVERSSHCONNECTION_H
#define CSOURCESERVERSSHCONNECTION_H


#include <QMap>

#include "CSourceServerCommand.h"

#include "IDataSourcesSender.h"

#include <ssh/sshremoteprocess.h>
#include <ssh/sshconnection.h>

class CSourceServerAuthorization;

class CSourceServerSshConnection : public QObject, public IDataSourcesSender
{
  Q_OBJECT
  Q_ENUMS(RemoteProcessExitStatus)
public:
  enum RemoteProcessExitStatus {FailedToStart, CrashExit, NormalExit};

  CSourceServerSshConnection(const QString& serverId,
                             const CSourceServerAuthorization& sourceServersAuthorization,
                             const QMap<QString, CSourceServerCommand>& commands,
                             const int connectionTimeout,
                             QObject* pParent = nullptr);
  virtual ~CSourceServerSshConnection() override;

  void connectToHost();

  bool isConnected() const;

  const QString& serverId() const;

  void killConnection();

signals:
  void connectedToHost(QString serverId) override;
  void disconnectedFromHost(QString serverId) override;
  void hostError(QString serverId, QString errorString) override;

  void newStandardStreamData(QString serverId, QString commandId, QByteArray data) override;
  void newErrorStreamData(QString serverId, QString commandId, QByteArray data) override;

  void commandStarted(QString serverId, QString commandId, QString outputExtension) override;
  void commandWasExit(QString serverId, QString commandId, RemoteCommandExitStatus exitStatus, int exitCode) override;

private slots:
  void onHostConnected();
  void onHostDisconnected();
  void onHostError();


  void onNewStandardStreamData();
  void onNewErrorStreamData();

  void onCommandStarted();
  void onCommandWasExit(int exitStatus);


  void closeConnection();

private:
  void startRemoteProcess(const QString& commandId);

  QSsh::SshConnectionParameters convertToSshConnectionParameters(const CSourceServerAuthorization& sourceServersAuthorization) const;

  const QString m_serverId;
  const int m_connectionTimeout;
  QSsh::SshConnection* const m_pSshConnection;
  QSharedPointer<QSsh::SshRemoteProcess> m_pKillChildsProcess;
  const QMap<QString, CSourceServerCommand> m_sourceServerCommands;
  QMap<QString, QSharedPointer<QSsh::SshRemoteProcess> > m_remoteProcesses;


};

#endif // CSOURCESERVERSSHCONNECTION_H
