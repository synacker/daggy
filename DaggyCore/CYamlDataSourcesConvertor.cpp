/*
Copyright 2017-2020 Milovidov Mikhail

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "Precompiled.h"
#include "CYamlDataSourcesConvertor.h"

#include "Command.h"
#include "Common.h"

using namespace daggycore;
using namespace daggyconv;

namespace YAML {

template<>
struct convert<QVariant>
{
    static bool decode(const Node& node, QVariant& rhs) {
        if (!node.IsScalar()) {
          return false;
        }

        rhs = QVariant(node);
        return true;
    }
};

// QString
template<>
struct convert<QString>
{
    static bool decode(const Node& node, QString& result)
    {
        if (!node.IsScalar())
            return false;
        result = QString::fromStdString(node.Scalar());
        return true;
    }
};

template<class K, class V>
struct convert<QMap<K, V>>
{
    static bool decode(const Node& node, QMap<K, V>& rhs) {
        if (!node.IsMap()) {
          return false;
        }

        std::map<K, V> smap = node.as<std::map<K, V>>();
        rhs = QMap<K, V>(smap);

        return true;
    }
};

template<>
struct convert<QVariantMap>
{
    static bool decode(const Node& node, QVariantMap& rhs) {
        if (!node.IsMap()) {
          return false;
        }

        const QMap<QString, QVariant> smap = node.as<QMap<QString, QVariant>>();
        rhs = QVariantMap(smap);

        return true;
    }
};

template<>
struct convert<daggycore::DataSources>
{
    static bool decode(const Node& node, daggycore::DataSources& rhs) {
        if (!node.IsMap()) {
          return false;
        }

        for (const auto& source : node) {
            daggycore::DataSource data_source;
            data_source.id = source.first.as<QString>();

            const auto& source_fields = source.second;
            data_source.host = source_fields[IDataSourceConvertor::g_hostField].as<QString>();
            data_source.type = source_fields[IDataSourceConvertor::g_typeField].as<QString>();

            if (source_fields[IDataSourceConvertor::g_reconnectField])
                data_source.reconnect = source_fields[IDataSourceConvertor::g_reconnectField].as<bool>();

            if (source_fields[IDataSourceConvertor::g_parametersField])
                data_source.parameters = source_fields[IDataSourceConvertor::g_parametersField].as<QVariantMap>();

            for (const auto& commands_yaml : source_fields[IDataSourceConvertor::g_commandsField]) {
                daggycore::Commands commands;
                for (const auto& command_yaml : commands_yaml) {
                    daggycore::Command command;
                    const QString& command_id = command_yaml.first.as<QString>();
                    command.id = command_id;

                    const auto& command_fields = command_yaml.second;
                    command.extension = command_fields[IDataSourceConvertor::g_extensionField].as<QString>();
                    command.exec = command_fields[IDataSourceConvertor::g_execField].as<QString>();

                    if(command_fields[IDataSourceConvertor::g_restartField])
                        command.restart = command_fields[IDataSourceConvertor::g_restartField].as<bool>();
                    commands[command.id] = command;
                }
                data_source.commands = commands;
            }
            rhs[data_source.id] = data_source;
        }
        return true;
    }
};
}

CYamlDataSourcesConvertor::CYamlDataSourcesConvertor()
    : IDataSourceConvertor(convertor_type)
{

}

OptionalResult<DataSources> CYamlDataSourcesConvertor::convert(const QString& yaml) const
{
    YAML::Node root_node = YAML::Load(yaml.toStdString());
    if (!root_node[g_sourcesField])
        return
        {
            DaggyErrors::ConvertError,
            QString("%1 field is absent in yaml").arg(g_sourcesField).toStdString()
        };
    DataSources data_sources;
    try {
        data_sources = root_node[g_sourcesField].as<DataSources>();
    } catch (const YAML::Exception& exception) {
        return Result
        {
            DaggyErrors::ConvertError,
            exception.what()
        };
    }
    return data_sources;
}
