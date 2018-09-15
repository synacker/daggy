/*
Copyright 2017-2018 Milovidov Mikhail

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "Precompiled.h"
#include "CSourceServer.h"

const QString& CSourceServer::g_passwordAuthorizationField("passwordAuthorization");
const QString& CSourceServer::g_authorizationField("authorization");
const QString& CSourceServer::g_commandsField("commands");

CSourceServer::CSourceServer(const QVariantMap& sourceServerMap)
{
  if (!sourceServerMap.isEmpty())
    initialize(sourceServerMap);
}

CSourceServer::CSourceServer(const CSourceServerAuthorization& sourceServerPasswordAuthorization)
  : m_sourceServerAuthorization(sourceServerPasswordAuthorization)
{

}

void CSourceServer::setSourceServerAuthorization(const CSourceServerAuthorization& sourceServerPasswordAuthorization)
{
  m_sourceServerAuthorization = sourceServerPasswordAuthorization;
}

const CSourceServerAuthorization& CSourceServer::sourceServerAuthorization() const
{
  return m_sourceServerAuthorization;
}

void CSourceServer::setSourceServerCommand(const QString& commandId, const CSourceServerCommand& sourceServerCommand)
{
  m_sourceServerCommands[commandId] = sourceServerCommand;
}

CSourceServerCommand CSourceServer::sourceServerCommand(const QString& commandId) const
{
  return m_sourceServerCommands[commandId];
}

void CSourceServer::removeSourceServerCommand(const QString& commandId)
{
  if (isExistCommandId(commandId))
    m_sourceServerCommands.remove(commandId);
}

bool CSourceServer::isExistCommandId(const QString& commandId) const
{
  return m_sourceServerCommands.contains(commandId);
}

QVariantMap CSourceServer::toSourceServerMap() const
{
  QVariantMap commands;
  for (const QString& commandId : m_sourceServerCommands.keys()) {
    commands[commandId] = m_sourceServerCommands[commandId].sourceServerCommandMap();
  }
  return {
          {g_authorizationField, m_sourceServerAuthorization.sourceServerAuthorizationMap()},
          {g_commandsField, commands}
  };
}

const QMap<QString, CSourceServerCommand>&CSourceServer::sourceServerCommands() const
{
  return m_sourceServerCommands;
}

void CSourceServer::initialize(const QVariantMap& sourceServerMap)
{
  QVariantMap authorizationMap = sourceServerMap[g_authorizationField].toMap();
  if (authorizationMap.isEmpty())
     authorizationMap = sourceServerMap[g_passwordAuthorizationField].toMap();
  m_sourceServerAuthorization = CSourceServerAuthorization(authorizationMap);

  const QVariantMap& sourceServerCommands = sourceServerMap[g_commandsField].toMap();
  for (const QString& commandId : sourceServerCommands.keys()) {
    setSourceServerCommand(commandId, CSourceServerCommand(sourceServerCommands[commandId].toMap()));
  }
}
