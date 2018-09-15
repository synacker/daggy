/*
Copyright 2017-2018 Milovidov Mikhail

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "Precompiled.h"
#include "CDataSources.h"

#include "CSourceServer.h"


CDataSources::CDataSources(const QVariantMap& dataSourcesMap)
{
  if (!dataSourcesMap.isEmpty())
    initialize(dataSourcesMap);
}

CDataSources::~CDataSources()
{
  qDeleteAll(m_sourceServers);
}

void CDataSources::removeSourceServer(const QString& serverId)
{
  if (isSourceServerExist(serverId)) {
    const CSourceServer* pSourceServer = m_sourceServers[serverId];
    delete pSourceServer;
    m_sourceServers.remove(serverId);
  }
}

void CDataSources::setSourceServer(const QString& serverId, CSourceServer* pSourceServer)
{
  m_sourceServers[serverId] = pSourceServer;
}

CSourceServer* CDataSources::sourceServer(const QString &serverId) const
{
  CSourceServer* pResult = nullptr;
  if (isSourceServerExist(serverId))
    pResult = m_sourceServers[serverId];
  return pResult;
}

QStringList CDataSources::sourceServers() const
{
  return m_sourceServers.keys();
}

bool CDataSources::isSourceServerExist(const QString& serverId) const
{
  return m_sourceServers.contains(serverId);
}


bool CDataSources::isEmpty() const
{
  return m_sourceServers.isEmpty();
}

QVariantMap CDataSources::toDataSourcesMap() const
{
  QVariantMap result;
  for (const QString& serverId : m_sourceServers.keys())
    result[serverId] = m_sourceServers[serverId]->toSourceServerMap();
  return result;
}

void CDataSources::initialize(const QVariantMap& sourceServersMap)
{
  for (const QString& serverId : sourceServersMap.keys()) {
    setSourceServer(serverId, new CSourceServer(sourceServersMap[serverId].toMap()));
  }
}
