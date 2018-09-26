#ifndef DATASOURCE_H
#define DATASOURCE_H

#include <QString>
#include <QVariantMap>
#include <vector>

#include "RemoteCommand.h"

struct DataSource {
    const QString serverName;
    const QString connectionType;
    const std::vector<RemoteCommand> remoteCommands;
    const QVariantMap connectionParameters;
};


using DataSources = std::vector<DataSource>;

DataSources convertDataSources(const QVariantMap& dataSources);


#endif // DATASOURCE_H
