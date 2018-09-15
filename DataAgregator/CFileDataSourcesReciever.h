/*
Copyright 2017-2018 Milovidov Mikhail

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef CFILEDATASOURCESRECIEVER_H
#define CFILEDATASOURCESRECIEVER_H

#include <QObject>
#include <QMap>
#include <QMetaEnum>

#include <DataAgregatorCore/IDataSourcesReciever.h>

class CApplicationSettings;
class QFile;

class CFileDataSourcesReciever : public IDataSourcesReciever
{
  Q_OBJECT
  Q_ENUMS(ConsoleMessageType)
public:
  enum ConsoleMessageType {StdError, ConnStatus, CommStatus};

  CFileDataSourcesReciever(const CApplicationSettings& applicationSettings, QObject* pParent = nullptr);
  ~CFileDataSourcesReciever();
public slots:

  void onConnectedToHost(QString serverId) override;
  void onDisconnectedFromHost(QString serverId) override;
  void onHostError(QString serverId, QString errorString) override;

  void onNewStandardStreamData(QString serverId, QString commandId, QByteArray data) override;
  void onNewErrorStreamData(QString serverId, QString commandId, QByteArray data) override;

  void onCommandStarted(QString serverId, QString commandId, QString outputExtension) override;
  void onCommandWasExit(QString serverId, QString commandId, DataSourcesRemoteProcessExitStatus exitStatus, int exitCode) override;

private:
  void printServerMessage(const ConsoleMessageType& messageType, const QString& serverId, const QString& serverMessage);
  void printCommandMessage(const ConsoleMessageType& messageType, const QString& serverId, const QString& commandId, const QString& commandMessage);
  QString currentConsoleTime() const;

  QString createOutputFolder(const QString& outputFolderPath) const;
  QString getOutputFilePath(const QString& serverId, const QString& commandId, const QString& outputExtension) const;
  void createOutputFile(QString serverId, QString commandId, QString outputExtension);
  void closeOutputFile(const QString& serverId, const QString& commandId);

  const QString m_outputFolderPath;
  QMap<QString, QMap<QString, QFile*>> m_outputFiles;
  QMetaEnum m_consoleMessageType;
};

#endif // CFILEDATASOURCESRECIEVER_H
