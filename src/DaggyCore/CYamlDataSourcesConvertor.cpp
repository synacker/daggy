/*
MIT License

Copyright (c) 2020 Mikhail Milovidov

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#include "Precompiled.h"
#include "CYamlDataSourcesConvertor.h"

#include "Command.h"
#include "Common.h"

using namespace daggycore;
using namespace daggyconv;

namespace YAML {

namespace  {
static const QRegularExpression yaml_scalar_true_values( "true|True|TRUE|on|On|ON" );
static const QRegularExpression yaml_scalar_false_values( "false|False|FALSE|off|Off|OFF" );
}

QVariant
scalarToVariant( const YAML::Node& scalarNode )
{
    const QString string = QString::fromStdString( scalarNode.as< std::string >() );
    if ( yaml_scalar_true_values.match( string ).capturedLength() == string.size() )
    {
        return QVariant( true );
    }
    if ( yaml_scalar_false_values.match( string ).capturedLength() == string.size())
    {
        return QVariant( false );
    }
    if ( QRegularExpression( "[-+]?\\d+" ).match( string ).capturedLength() == string.size() )
    {
        return QVariant( string.toLongLong() );
    }
    if ( QRegularExpression( "[-+]?\\d*\\.?\\d+" ).match( string ).capturedLength() == string.size() )
    {
        return QVariant( string.toDouble() );
    }
    return QVariant( string );
}

template<>
struct convert<QVariant>
{
    static bool decode(const Node& node, QVariant& rhs) {
        switch (node.Type()) {
        case YAML::NodeType::Undefined:
        case YAML::NodeType::Null:
            rhs = QVariant();
            break;
        case YAML::NodeType::Scalar:
            rhs = scalarToVariant(node);
            break;
        case YAML::NodeType::Sequence:
        {
            QVariantList sequence;
            for ( YAML::const_iterator it = node.begin(); it != node.end(); ++it )
            {
                QVariant value;
                if (decode( *it, value ))
                    sequence << value;
                else
                    return false;
            }
            rhs = sequence;
        }
            break;
        case YAML::NodeType::Map:
        {
            QVariantMap map;
            for ( YAML::const_iterator it = node.begin(); it != node.end(); ++it )
            {
                QVariant value;
                if (decode(it->second, value))
                    map.insert( QString::fromStdString( it->first.as< std::string >() ), value );
                else
                    return false;
            }
            rhs = map;
        }
            break;
        }
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
            if (source_fields[IDataSourceConvertor::g_hostField])
                data_source.host = source_fields[IDataSourceConvertor::g_hostField].as<QString>();

            data_source.type = source_fields[IDataSourceConvertor::g_typeField].as<QString>();

            if (source_fields[IDataSourceConvertor::g_reconnectField])
                data_source.reconnect = source_fields[IDataSourceConvertor::g_reconnectField].as<bool>();

            if (source_fields[IDataSourceConvertor::g_parametersField])
                data_source.parameters = source_fields[IDataSourceConvertor::g_parametersField].as<QVariantMap>();

            const auto& commands = IDataSourceConvertor::getCommands(source_fields[IDataSourceConvertor::g_commandsField].as<QVariantMap>());
            if (!commands) {
                throw std::runtime_error(commands.result().detailed_error_message());
                return false;
            }

            if (commands.value().size() == 0)
                throw std::invalid_argument
                {
                    QString("%1 data source don't have commands").
                            arg(data_source.id).toStdString()
                };

            data_source.commands = commands.value();
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
            QString("Yaml have syntax error: %1").arg(exception.what()).toStdString()
        };
    } catch (const std::exception& exception) {
        return Result
        {
            DaggyErrors::ConvertError,
            QString("Yaml error: %1").arg(exception.what()).toStdString()
        };
    }

    return data_sources;
}
