#include "Precompiled.h"
#include "CDataSourcesFabric.h"

using namespace dataagregatorcore;


namespace {

constexpr const char* g_typeField = "type";
constexpr const char* g_hostField = "host";
constexpr const char* g_authorizationField = "authorization";
constexpr const char* g_passwordAuthorizationField = "passwordAuthorization";
constexpr const char* g_commandsField = "commands";
constexpr const char* g_reconnectField = "reconnect";

constexpr const char* g_commandField = "command";
constexpr const char* g_outputExtensionField = "outputExtension";
constexpr const char* g_restart = "restart";

}

DataSources CDataSourcesFabric::getDataSources(const CDataSourcesFabric::InputTypes input_type, const QString& input)
{
    DataSources result;
    switch (input_type) {
    case InputTypes::Json:
        result = getFromJson(input);
        break;
    case InputTypes::Yaml:
        break;
    }
    return result;
}

DataSources CDataSourcesFabric::getFromJson(const QString& json)
{
    QJsonParseError json_parse_error;
    QJsonDocument task_document = QJsonDocument::fromJson(json.toUtf8(), &json_parse_error);

    if (json_parse_error.error != QJsonParseError::NoError)
        qFatal("Cann't parse json: %s", qPrintable(json_parse_error.errorString()));

    return convertDataSources(task_document.toVariant().toMap());
}


DataSources CDataSourcesFabric::convertDataSources(const QVariantMap& data_sources)
{
    DataSources result;

    for (const QString& serverName : data_sources.keys()) {
        const QVariantMap& dataSource = data_sources[serverName].toMap();
        ValidateField(!dataSource.isEmpty(), errorMessage(serverName, "invalidate parameters format"));

        const QString& connectionType = dataSource[g_typeField].toString();
        const QString& host = dataSource[g_hostField].toString();

        QVariantMap connection = dataSource[g_authorizationField].toMap();
        if (connection.isEmpty())
            connection = dataSource[g_passwordAuthorizationField].toMap();

        const QVariantMap& commands = dataSource[g_commandsField].toMap();
        ValidateField(!commands.isEmpty(), errorMessage(serverName, QString("%1 field is absent").arg(g_commandsField)));

        const bool reconnect = dataSource[g_reconnectField].toBool();

        const std::vector<RemoteCommand>& remoteCommands = getCommands(commands, serverName);

        result.push_back({serverName, connectionType, host, remoteCommands, connection, reconnect});
    }

    return result;
}

QString CDataSourcesFabric::errorMessage(const QString& serverName, const QString& error)
{
    return QString("Error for server name %1 - %2").arg(serverName).arg(error);
}

bool CDataSourcesFabric::ValidateField(const bool isOk, const QString& errorMessage)
{
    if (!isOk)
        throw std::runtime_error(qPrintable(errorMessage));
    return isOk;
}

std::vector<RemoteCommand> CDataSourcesFabric::getCommands(const QVariantMap& commands, const QString& serverName)
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
