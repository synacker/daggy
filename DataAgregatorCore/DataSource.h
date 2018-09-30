#ifndef DATASOURCE_H
#define DATASOURCE_H

#include <QString>
#include <QVariantMap>
#include <vector>

#include "RemoteCommand.h"
#include "dataagregatorcore_global.h"

struct DataSource;
using DataSources = std::vector<DataSource>;

struct DATAAGREGATORCORESHARED_EXPORT DataSource {
    const QString server_name;
    const QString connection_type;
    const std::vector<RemoteCommand> remote_commands;
    const QVariantMap connection_parameters;
    const bool reconnect;

    static DataSources convertDataSources(const QVariantMap& data_sources);

private:
    static bool ValidateField(const bool isOk, const QString& errorMessage);
    static QString errorMessage(const QString& serverName, const QString& error);
    static std::vector<RemoteCommand> getCommands(const QVariantMap& commands, const QString& serverName);
};






#endif // DATASOURCE_H
