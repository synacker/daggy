/*
Copyright 2017-2018 Milovidov Mikhail

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef CSOURCESERVER_H
#define CSOURCESERVER_H


#include "dataagregatorcore_global.h"

#include <QString>
#include <QMap>
#include <QVariantMap>

#include "CSourceServerCommand.h"
#include "CSourceServerAuthorization.h"


class CSourceServerCommand;
class CSourceServerAuthorization;

class CSourceServer
{
public:
  CSourceServer(const QVariantMap& sourceServerMap);
  CSourceServer(const CSourceServerAuthorization& sourceServerAuthorization);

  void setSourceServerAuthorization(const CSourceServerAuthorization& sourceServerAuthorization);
  const CSourceServerAuthorization& sourceServerAuthorization() const;

  void setSourceServerCommand(const QString& commandId, const CSourceServerCommand& sourceServerCommand);
  CSourceServerCommand sourceServerCommand(const QString& commandId) const;
  void removeSourceServerCommand(const QString& commandId);

  bool isExistCommandId(const QString& commandId) const;
  QStringList commandIds() const;

  QVariantMap toSourceServerMap() const;

  const QMap<QString, CSourceServerCommand>& sourceServerCommands() const;


  static const QString& g_passwordAuthorizationField;
  static const QString& g_authorizationField;
  static const QString& g_commandsField;

private:
  void initialize(const QVariantMap& sourceServerMap);



  CSourceServerAuthorization m_sourceServerAuthorization;
  QMap<QString, CSourceServerCommand> m_sourceServerCommands;
};

#endif // CSOURCESERVER_H
