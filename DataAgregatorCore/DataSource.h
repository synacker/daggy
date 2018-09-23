#ifndef DATASOURCE_H
#define DATASOURCE_H

#include <QString>
#include <QVector>
#include <QVariantMap>
#include "RemoteCommand.h"

struct DataSource {
    const QString serverName;
    const QString connectionType;
    const QVector<RemoteCommand> remoteCommands;
    const QVariantMap connectionParameters;
};


using DataSources = QVector<DataSource>;

DataSources convertDataSources(const QVariantMap& dataSources);


#endif // DATASOURCE_H
