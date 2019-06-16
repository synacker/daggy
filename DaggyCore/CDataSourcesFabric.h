/*
Copyright 2016-2019 Mikhail Milovidov <milovidovmikhail@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once

#include "DataSource.h"
#include "daggycore_global.h"
#include <QMetaEnum>

namespace YAML {
    class Node;
}

namespace daggycore {

class DAGGYCORESHARED_EXPORT CDataSourcesFabric
{
    Q_GADGET
public:
    enum DataSourcesType {json, yaml};
    Q_ENUM(DataSourcesType)

    CDataSourcesFabric();

    DataSources getDataSources(const DataSourcesType input_type, const QString& input) const;

    QStringList supportedSourceTypes() const;

    QString inputTypeName(const DataSourcesType input_type) const;
    DataSourcesType sourceTypeValue(const QString& input_type_name) const;

    bool isSourceTypeSopported(const QString& input_type) const;

private:
    DataSources getFromJson(const QString& json) const;
    DataSources getFromYaml(const QString& yaml) const;

    QVariantMap getConnectionParameters(const YAML::Node& node) const;
    QVariantMap getCommands(const QString& server_name, const YAML::Node& node) const;

    QVariantMap parseYamlServerSource(const QString& server_name, const YAML::Node& node) const;
    QVariantMap parseStringYamlNode(const YAML::Node& node) const;
    QString safeRead(const QString& field, const YAML::Node& node) const;

    DataSources convertDataSources(const QVariantMap& data_sources) const;
    QString sourceErrorMessage(const QString& serverName, const QString& error) const;
    bool ValidateField(const bool isOk, const QString& sourceErrorMessage) const;
    std::vector<RemoteCommand> getCommands(const QVariantMap& commands, const QString& serverName) const;

    QMetaEnum data_sources_type_;
};

}
