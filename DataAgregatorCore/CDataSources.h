/*
Copyright 2017-2018 Milovidov Mikhail

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef CDATASOURCES_H
#define CDATASOURCES_H

#include "dataagregatorcore_global.h"

#include <QString>
#include <QStringList>
#include <QMap>
#include <QVariantMap>

class CSourceServer;

class DATAAGREGATORCORESHARED_EXPORT CDataSources
{
public:
  CDataSources(const QVariantMap& dataSourcesMap = QVariantMap());
  ~CDataSources();

  CSourceServer* sourceServer(const QString &serverId) const;


  QStringList sourceServers() const;
  bool isSourceServerExist(const QString& serverId) const;
  bool isEmpty() const;

  QVariantMap toDataSourcesMap() const;

private:
  void initialize(const QVariantMap& sourceServersMap);
  void setSourceServer(const QString& serverId, CSourceServer* pSourceServer);
  void removeSourceServer(const QString& serverId);

  QMap<QString, CSourceServer*> m_sourceServers;

};

#endif // CDATASOURCES_H
