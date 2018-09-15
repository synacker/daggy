/*
Copyright 2017-2018 Milovidov Mikhail

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "Precompiled.h"
#include "CFileDataSourcesReciever.h"
#include "CApplicationSettings.h"


CFileDataSourcesReciever::CFileDataSourcesReciever(const CApplicationSettings& applicationSettings, QObject* pParent)
  : IDataSourcesReciever(pParent)
  , m_outputFolderPath(createOutputFolder(applicationSettings.outputFolder()))
{
  const QMetaObject* const pThisMetaObject = metaObject();
  m_consoleMessageType = pThisMetaObject->enumerator(pThisMetaObject->indexOfEnumerator("ConsoleMessageType"));
}

CFileDataSourcesReciever::~CFileDataSourcesReciever()
{
  for (const QString& serverId : m_outputFiles.keys())
    for (const QString& commandId : m_outputFiles[serverId].keys())
      closeOutputFile(serverId, commandId);
}

void CFileDataSourcesReciever::onConnectedToHost(QString serverId)
{
  printServerMessage(ConnStatus, serverId, "Server connected");
}

void CFileDataSourcesReciever::onDisconnectedFromHost(QString serverId)
{
  printServerMessage(ConnStatus, serverId, "Server disconnected");
}

void CFileDataSourcesReciever::onHostError(QString serverId, QString errorString)
{
  printServerMessage(ConnStatus, serverId, errorString);
}

void CFileDataSourcesReciever::onNewStandardStreamData(QString serverId, QString commandId, QByteArray data)
{
  if (m_outputFiles[serverId].contains(commandId)) {
    QFile* pOutputFile = m_outputFiles[serverId][commandId];
    if (pOutputFile) {
      pOutputFile->write(data);
      pOutputFile->flush();
    }
  }
}

void CFileDataSourcesReciever::onNewErrorStreamData(QString serverId, QString commandId, QByteArray data)
{
  const QString errorMessage(data);
  if (errorMessage.length() > 0)
    printCommandMessage(StdError, serverId, commandId, errorMessage);
}

void CFileDataSourcesReciever::onCommandStarted(QString serverId, QString commandId, QString outputExtension)
{
  printCommandMessage(CommStatus, serverId, commandId, "Command started");
  createOutputFile(serverId, commandId, outputExtension);
}

void CFileDataSourcesReciever::onCommandWasExit(QString serverId, QString commandId, DataSourcesRemoteProcessExitStatus exitStatus, int exitCode)
{
  switch (exitStatus) {
  case FailedToStart:
    printCommandMessage(CommStatus, serverId, commandId, "Cann't start command");
    break;
  case CrashExit:
    printCommandMessage(CommStatus, serverId, commandId, "Command was crashed");
    break;
  case NormalExit:
    printCommandMessage(CommStatus, serverId, commandId, QString("Command was finished with code %2").arg(exitCode));
  default:
    break;
  }

  closeOutputFile(serverId, commandId);
}

void CFileDataSourcesReciever::printServerMessage(const CFileDataSourcesReciever::ConsoleMessageType& messageType, const QString& serverId, const QString& serverMessage)
{
  const char* pMessageTypeString = m_consoleMessageType.valueToKey(messageType);
  printf("%12s | %-10s | %-15s | %s\n", qPrintable(currentConsoleTime()), pMessageTypeString, qPrintable(serverId), qPrintable(serverMessage));
}

void CFileDataSourcesReciever::printCommandMessage(const CFileDataSourcesReciever::ConsoleMessageType& messageType, const QString& serverId, const QString& commandId, const QString& commandMessage)
{
  const char* pMessageTypeString = m_consoleMessageType.valueToKey(messageType);
  printf("%12s | %-10s | %-15s | %-15s | %s\n", qPrintable(currentConsoleTime()), pMessageTypeString, qPrintable(serverId), qPrintable(commandId), qPrintable(commandMessage));
}

QString CFileDataSourcesReciever::currentConsoleTime() const
{
  return QDateTime::currentDateTime().toString("hh:mm:ss:zzz");
}

QString CFileDataSourcesReciever::createOutputFolder(const QString& outputFolderPath) const
{
  QString result;
  if (outputFolderPath.isEmpty()) {
    result = QDateTime::currentDateTime().toString("dd-MM-yy_hh-mm-ss") + "_output";
  } else {
    result = outputFolderPath;
  }
  QDir outputFolder(result);
  result = outputFolder.absolutePath();
  if (!outputFolder.exists() &&
      !outputFolder.mkpath(result)) {
    qFatal("Cann't create output folder %s", qPrintable(result));
  }
  return result;
}

QString CFileDataSourcesReciever::getOutputFilePath(const QString& serverId, const QString& commandId, const QString& outputExtension) const
{
  return QString("%1/%2_%3.%4").arg(m_outputFolderPath, serverId, commandId, outputExtension);
}

void CFileDataSourcesReciever::closeOutputFile(const QString& serverId, const QString& commandId)
{
  if (!m_outputFiles[serverId].contains(commandId)) {
    QFile* pOutputFile = m_outputFiles[serverId][commandId];
    m_outputFiles[serverId].remove(commandId);
    if (m_outputFiles[serverId].isEmpty())
      m_outputFiles.remove(serverId);
    if (pOutputFile)
      pOutputFile->close();
    delete pOutputFile;
  }
}

void CFileDataSourcesReciever::createOutputFile(QString serverId, QString commandId, QString outputExtension)
{
  if (!m_outputFiles[serverId].contains(commandId)) {
    const QString& filePath = getOutputFilePath(serverId, commandId, outputExtension);
    QFile* pOutputFile = new QFile(filePath);
    if (!pOutputFile->open(QIODevice::Append)) {
      qWarning() << QString("Cannot open file %1 for writing").arg(filePath);
      delete pOutputFile;
      pOutputFile = nullptr;
    }
    if (pOutputFile)
      m_outputFiles[serverId][commandId] = pOutputFile;
  }
}
