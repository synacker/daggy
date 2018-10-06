/*
Copyright 2017-2018 Milovidov Mikhail

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef DATASOURCE_H
#define DATASOURCE_H

#include <QString>
#include <QVariantMap>
#include <vector>

#include "RemoteCommand.h"
#include "dataagregatorcore_global.h"

namespace dataagregatorcore {

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

}




#endif // DATASOURCE_H
