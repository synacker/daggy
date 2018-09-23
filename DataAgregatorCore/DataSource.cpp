#include "Precompiled.h"
#include "DataSource.h"

namespace {

constexpr const char* g_typeField = "type";
constexpr const char* g_connectionField = "connection";
constexpr const char* g_connectionOldField = "passwordAuthorization";
constexpr const char* g_commandsField = "commands";

constexpr const char* g_commandField = "command";
constexpr const char* g_outputExtensionField = "outputExtension";

}

bool ValidateField(const bool isOk, const QString& errorMessage);
QString errorMessage(const QString& serverName, const QString& error);

DataSources convertDataSources(const QVariantMap& dataSources)
{
    DataSources result;

    for (const QString& serverName : dataSources.keys()) {
        const QVariantMap& dataSource = dataSources[serverName].toMap();
        ValidateField(!dataSource.isEmpty(), errorMessage(serverName, "invalidate parameters format"));

        const QString& connectionType = dataSource[g_typeField].toString();

        QVariantMap connection = dataSource[g_connectionField].toMap();
        if (connection.isEmpty())
            connection = dataSource[g_connectionOldField].toMap();

        QVector<RemoteCommand> remoteCommands;
        const QVariantMap& commands = dataSource[g_commandsField].toMap();
        ValidateField(!commands.isEmpty(), errorMessage(serverName, QString("%1 field is absent").arg(g_commandsField)));

        for (const QString& commandName : commands.keys()) {
            const QVariantMap& remoteCommandParameters = commands[commandName].toMap();
            const QString& command = remoteCommandParameters[g_commandField].toString();
            const QString& outputExtension = remoteCommandParameters[g_outputExtensionField].toString();
            ValidateField(!command.isEmpty(),
                          errorMessage(serverName, QString("%1 field is absent for %2").arg(g_commandField).arg(commandName)));
            ValidateField(!outputExtension.isEmpty(),
                          errorMessage(serverName, QString("%1 field is absent for %2").arg(g_outputExtensionField).arg(commandName)));

            remoteCommands.push_back({commandName, command, outputExtension});
        }

        result.push_back({serverName, connectionType, remoteCommands, connection});
    }

    return result;
}

bool ValidateField(const bool isOk, const QString& errorMessage)
{
    if (!isOk)
        throw std::runtime_error(qPrintable(errorMessage));
    return isOk;
}

QString errorMessage(const QString& serverName, const QString& error)
{
    return QString("Error for server name %1 - %2").arg(serverName).arg(error);
}
