/*
Copyright 2017-2018 Milovidov Mikhail

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "Precompiled.h"
#include "DataSource.h"

using namespace dataagregatorcore;

namespace {

constexpr const char* g_typeField = "type";
constexpr const char* g_connectionField = "connection";
constexpr const char* g_connectionOldField = "passwordAuthorization";
constexpr const char* g_commandsField = "commands";
constexpr const char* g_reconnectField = "reconnect";

constexpr const char* g_commandField = "command";
constexpr const char* g_outputExtensionField = "outputExtension";
constexpr const char* g_restart = "restart";

}

DataSources DataSource::convertDataSources(const QVariantMap& data_sources)
{
    DataSources result;

    for (const QString& serverName : data_sources.keys()) {
        const QVariantMap& dataSource = data_sources[serverName].toMap();
        ValidateField(!dataSource.isEmpty(), errorMessage(serverName, "invalidate parameters format"));

        const QString& connectionType = dataSource[g_typeField].toString();

        QVariantMap connection = dataSource[g_connectionField].toMap();
        if (connection.isEmpty())
            connection = dataSource[g_connectionOldField].toMap();

        const QVariantMap& commands = dataSource[g_commandsField].toMap();
        ValidateField(!commands.isEmpty(), errorMessage(serverName, QString("%1 field is absent").arg(g_commandsField)));

        const bool reconnect = dataSource[g_reconnectField].toBool();

        const std::vector<RemoteCommand>& remoteCommands = getCommands(commands, serverName);

        result.push_back({serverName, connectionType, remoteCommands, connection, reconnect});
    }

    return result;
}

bool DataSource::ValidateField(const bool isOk, const QString& errorMessage)
{
    if (!isOk)
        throw std::runtime_error(qPrintable(errorMessage));
    return isOk;
}

QString DataSource::errorMessage(const QString& serverName, const QString& error)
{
    return QString("Error for server name %1 - %2").arg(serverName).arg(error);
}

std::vector<RemoteCommand> DataSource::getCommands(const QVariantMap& commands, const QString& serverName)
{
    std::vector<RemoteCommand> result;
    for (const QString& commandName : commands.keys()) {
        const QVariantMap& remoteCommandParameters = commands[commandName].toMap();
        const QString& command = remoteCommandParameters[g_commandField].toString();
        const QString& outputExtension = remoteCommandParameters[g_outputExtensionField].toString();
        ValidateField(!command.isEmpty(),
                      errorMessage(serverName, QString("%1 field is absent for %2").arg(g_commandField).arg(commandName)));
        ValidateField(!outputExtension.isEmpty(),
                      errorMessage(serverName, QString("%1 field is absent for %2").arg(g_outputExtensionField).arg(commandName)));

        const bool restart = remoteCommandParameters.value(g_restart, false).toBool();

        result.push_back({commandName, command, outputExtension, restart});
    }
    return result;
}
