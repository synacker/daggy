/*
MIT License

Copyright (c) 2021 Mikhail Milovidov

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
#include "Precompiled.hpp"
#include "Sources.hpp"
#include "Result.hpp"
#include "Errors.hpp"

namespace  {
thread_local const char* g_sourcesField = "sources";

thread_local const char* g_hostField = "host";
thread_local const char* g_typeField = "type";
thread_local const char* g_parametersField = "parameters";
thread_local const char* g_commandsField = "commands";
thread_local const char* g_reconnectField = "reconnect";

thread_local const char* g_execField = "exec";
thread_local const char* g_extensionField = "extension";
thread_local const char* g_restartField = "restart";

thread_local const QHash<const char*, QMetaType::Type> required_source_field =
{
    {g_typeField, QMetaType::QString},
    {g_commandsField, QMetaType::Type::QVariantMap}
};

thread_local const QHash<const char*, QMetaType::Type> required_commands_field =
{
    {g_execField, QMetaType::QString},
    {g_extensionField, QMetaType::QString}
};

daggy::Result<daggy::sources::Commands> getCommands(const QVariantMap& commands_map)
{
    daggy::sources::Commands commands;
    const auto& keys = commands_map.keys();
    for (const QString& command_id : keys) {
        if (commands_map[command_id].metaType() != QMetaType(QMetaType::QVariantMap))
            return
            {
                daggy::errors::make_error_code(DaggyErrorSourceConvertion),
                QString("%1 command is not a map").arg(command_id)
            };
        const QVariantMap& command_map = commands_map[command_id].toMap();
        static const auto& requeried_fields = required_commands_field.keys();
        for (const auto& field : requeried_fields) {
            if (!command_map.contains(field)) {
                return
                {
                    daggy::errors::make_error_code(DaggyErrorSourceConvertion),
                    QString("%1 command don't have required %2 field").arg(command_id, field)
                };
            }
            if (command_map[field].metaType() != QMetaType(required_commands_field.value(field)))
            {
                return
                {
                    daggy::errors::make_error_code(DaggyErrorSourceConvertion),
                    QString("%1 command have incorrect type for %2 field").arg(command_id, field)
                };
            }
        }
        daggy::sources::Command command{command_id,
        {
            command_map[g_extensionField].toString(),
            command_map[g_execField].toString(),
            command_map[g_parametersField].toMap(),
            (command_map.contains(g_restartField) && command_map[g_restartField].metaType() == QMetaType(QMetaType::Bool)) ? command_map[g_restartField].value<bool>() : false,
        }};
        commands[command.first] = command.second;
    }
    return {std::move(commands)};
}
}

#ifdef YAML_SUPPORT
namespace YAML {

namespace  {
thread_local const QRegularExpression yaml_scalar_true_values( "true|True|TRUE|on|On|ON" );
thread_local const QRegularExpression yaml_scalar_false_values( "false|False|FALSE|off|Off|OFF" );
}

QVariant
scalarToVariant( const Node& scalarNode )
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
struct convert<daggy::Sources>
{
    static bool decode(const Node& node, daggy::Sources& rhs) {
        if (!node.IsMap()) {
          return false;
        }

        for (const auto& source : node) {
            auto source_id = source.first.as<QString>();
            daggy::sources::Properties properties;

            const auto& source_fields = source.second;
            if (source_fields[g_hostField])
                properties.host = source_fields[g_hostField].as<QString>();

            properties.type = source_fields[g_typeField].as<QString>();

            if (source_fields[g_reconnectField])
                properties.reconnect = source_fields[g_reconnectField].as<bool>();

            if (source_fields[g_parametersField])
                properties.parameters = source_fields[g_parametersField].as<QVariantMap>();

            const auto& commands = getCommands(source_fields[g_commandsField].as<QVariantMap>());
            if (!commands) {
                throw std::runtime_error(commands.error.message());
                return false;
            }

            if (commands->isEmpty())
                throw std::invalid_argument
                {
                    QString("%1 data source don't have commands").
                            arg(source_id).toStdString()
                };
            properties.commands = std::move(*commands);
            rhs[source_id] = std::move(properties);
        }
        return true;
    }
};
}
#endif

bool daggy::sources::commands::Properties::operator==(const Properties& other) const
{
    return extension == other.extension &&
           exec == other.exec &&
           parameters == other.parameters &&
           restart == other.restart;
}


bool daggy::sources::Properties::operator==(const Properties& other) const
{
    return type == other.type &&
           host == other.host &&
           commands == other.commands &&
           reconnect == other.reconnect &&
           parameters == other.parameters;
}

std::optional<daggy::Sources> daggy::sources::convertors::json(const QString& data, QString& error) noexcept
{
    Sources result;
    QVariantMap sources_map;
    {
        QVariantMap parsed_sources;
        {
            QJsonParseError json_parse_error;
            QJsonDocument task_document = QJsonDocument::fromJson
            (
                data.toUtf8(),
                &json_parse_error
            );
            if (json_parse_error.error != QJsonParseError::NoError)
            {
                error = json_parse_error.errorString();
                return {};
            }
            parsed_sources = task_document.toVariant().toMap();
        }

        if (!parsed_sources.contains(g_sourcesField))
        {
            error = QString("%1 field is absent").arg(g_sourcesField);
            return {};
        }
        if (parsed_sources[g_sourcesField].metaType() != QMetaType(QMetaType::QVariantMap))
        {
            error = QString("%1 field is not a map").arg(g_sourcesField);
            return {};
        }

        sources_map = parsed_sources[g_sourcesField].toMap();
    }


    for (const QString& source_id : sources_map.keys())
    {
        if (sources_map[source_id].metaType() != QMetaType(QMetaType::QVariantMap))
        {
            error = QString("%1 data source is not a map").arg(source_id);
            return {};
        }
        const QVariantMap& source_map = sources_map[source_id].toMap();
        for(const auto& field : required_source_field.keys())
        {
            if (!source_map.contains(field))
            {
                error = QString("%1 data source don't have required '%2' field").arg(source_id, field);
                return {};
            }
            if (source_map[field].metaType() != QMetaType(required_source_field.value(field)))
            {
                error = QString("%1 data source have incorrect type for '%2' field").arg(source_id, field);
                return {};
            }
        }
        const auto commands = getCommands(source_map[g_commandsField].toMap());
        if (!commands)
        {
            error = QString("%1 data source have incorrect commands. %2")
                    .arg(source_id, std::move(commands.message));
            return
            {};
        }

        if (commands->isEmpty())
        {
            error = QString("%1 data source don't have commands")
                    .arg(source_id);
            return {};
        }

        Source source{
            source_id,
            {
                source_map[g_typeField].toString(),
                source_map[g_hostField].toString(),
                std::move(*commands),
                source_map.contains(g_reconnectField) ? source_map[g_reconnectField].toBool() : false,
                (source_map.contains(g_parametersField) && source_map[g_parametersField].metaType() == QMetaType(QMetaType::QVariantMap) ? source_map[g_parametersField].toMap() : QVariantMap())
            }
        };
        result[source.first] = source.second;
    }
    return result;
}

#ifdef YAML_SUPPORT
std::optional<daggy::Sources> daggy::sources::convertors::yaml(const QString& data, QString& error) noexcept
try {
    YAML::Node root_node = YAML::Load(data.toStdString());
    if (!root_node[g_sourcesField].IsMap())
    {
        error = QString("%1 field is absent in yaml").arg(g_sourcesField);
        return {};
    }
    return root_node[g_sourcesField].as<Sources>();;
}
catch (const YAML::ParserException& exception) {
    error = QString::fromStdString(exception.msg);
    return {};
}
catch (const std::exception& exception) {
    error = QString::fromStdString(exception.what());
    return {};
}

#else
std::optional<daggy::Sources> daggy::sources::convertors::yaml(const QString& data, QString& error)
{
    error = "yaml not supported";
    return {};
}
#endif

std::optional<daggy::Sources> daggy::sources::convertors::json(const QString& data)
{
    QString error;
    auto result = json(data, error);
    if (!result)
        throw std::runtime_error(error.toStdString());
    return result;
}

std::optional<daggy::Sources> daggy::sources::convertors::yaml(const QString& data)
{
    QString error;
    auto result = yaml(data, error);
    if (!result)
        throw std::runtime_error(error.toStdString());
    return result;
}
