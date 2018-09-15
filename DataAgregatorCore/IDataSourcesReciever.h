/*
Copyright 2017-2018 Milovidov Mikhail

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef IDATASOURCESRECIEVER_H
#define IDATASOURCESRECIEVER_H

#include <QObject>

#include "DataSourcesTypes.h"

#include "dataagregatorcore_global.h"

class DATAAGREGATORCORESHARED_EXPORT IDataSourcesReciever : public QObject
{
  Q_OBJECT
public:
  IDataSourcesReciever(QObject* pParent = nullptr);
  virtual ~IDataSourcesReciever();

//slots:
  virtual void onConnectedToHost(QString serverId) = 0;
  virtual void onDisconnectedFromHost(QString serverId) = 0;
  virtual void onHostError(QString serverId, QString errorString) = 0;

  virtual void onNewStandardStreamData(QString serverId, QString commandId, QByteArray data) = 0;
  virtual void onNewErrorStreamData(QString serverId, QString commandId, QByteArray data) = 0;

  virtual void onCommandStarted(QString serverId, QString commandId, QString outputExtension) = 0;
  virtual void onCommandWasExit(QString serverId, QString commandId, DataSourcesRemoteProcessExitStatus exitStatus, int exitCode) = 0;
};



#endif // IDATASOURCESRECIEVER_H
