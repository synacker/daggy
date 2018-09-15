/*
Copyright 2017-2018 Milovidov Mikhail

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "Precompiled.h"
#include "CSourceServerCommand.h"

const QString& CSourceServerCommand::g_commandField("command");
const QString& CSourceServerCommand::g_outputExtensionField("outputExtension");


CSourceServerCommand::CSourceServerCommand()
{

}

CSourceServerCommand::CSourceServerCommand(const QVariantMap& sourceServerCommandMap)
  : m_sourceServerCommandMap(sourceServerCommandMap)
{
}

CSourceServerCommand::CSourceServerCommand(const QString& command, const QString& outputExtension)
  : m_sourceServerCommandMap({
                              {g_commandField, command},
                              {g_outputExtensionField, outputExtension}
                            })
{

}

QString CSourceServerCommand::command() const
{
  return m_sourceServerCommandMap[g_commandField].toString();
}

QString CSourceServerCommand::outputExtension() const
{
  return m_sourceServerCommandMap[g_outputExtensionField].toString();
}

const QVariantMap& CSourceServerCommand::sourceServerCommandMap() const
{
  return m_sourceServerCommandMap;
}
