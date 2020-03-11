/*
Copyright 2017-2020 Milovidov Mikhail

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "Precompiled.h"
#include "CJsonDataSourcesConvertor.h"

#include "Common.h"

using namespace daggycore;
using namespace daggyconv;

CJsonDataSourcesConvertor::CJsonDataSourcesConvertor()
    : IDataSourceConvertor(convertor_type)
{

}

OptionalResult<DataSources> CJsonDataSourcesConvertor::convert(
        const QString& json
) const
{
    QJsonParseError json_parse_error;
    QJsonDocument task_document = QJsonDocument::fromJson
    (
                json.toUtf8(),
                &json_parse_error
    );
    if (json_parse_error.error != QJsonParseError::NoError)
        return
        {
            DaggyErrors::ConvertError,
            json_parse_error.errorString().toStdString()
        };

    const QVariantMap& sources = task_document.toVariant().toMap();
    if (!sources.contains(g_sourcesField)) {
        return
        {
            DaggyErrors::ConvertError,
            QString("%1 field is absent").arg(g_sourcesField).toStdString()
        };
    }

    if (sources[g_sourcesField].type() != QVariant::Map) {
        return
        {
            daggycore::DaggyErrors::ConvertError,
            QString("%1 field is not a map").arg(g_sourcesField).toStdString()
        };
    }

    DataSources data_sources;
    const QVariantMap& sources_map = sources[g_sourcesField].toMap();
    for (const QString& source_id : sources_map.keys()) {
        DataSource source;
        source.id = source_id;

        if (sources_map[source_id].type() != QVariant::Map)
            return
            {
                daggycore::DaggyErrors::ConvertError,
                QString("%1 data source is not a map").arg(source_id).toStdString()
            };
        Commands commands;
        for(const char* field : required_source_field.keys()) {
            if (!sources_map.contains(field)) {
                return
                {
                    daggycore::DaggyErrors::ConvertError,
                    QString("%1 data source don't have required %2 field").arg(source_id, field).toStdString()
                };
            }
            if (sources_map[field].type() != required_source_field.value(field))
            {
                return
                {
                    daggycore::DaggyErrors::ConvertError,
                    QString("%1 data source have incorrect type for %2 field").arg(source_id, field).toStdString()
                };
            }
        }
        const auto commands_convertion = getCommands(sources_map[g_commandsField].toMap());
        if (!commands_convertion) {
            return
            {
                DaggyErrors::ConvertError,
                QString("%1 data source have incorrect commands. %2")
                        .arg(source_id,
                             QString::fromStdString(commands_convertion.result().detailed_error_message()))
                        .toStdString()
            };
        }

        const QVariantMap& source_map = sources_map[source_id].toMap();
        source.host = source_map[g_hostField].toString();
        source.type = source_map[g_typeField].toString();
        if (source_map.contains(g_reconnectField))
            source.reconnect = source_map[g_reconnectField].toBool();

        if(source_map.contains(g_parametersField) &&
           source_map[g_parametersField].type() == QVariant::Map)
        {
            source.parameters = source_map[g_parametersField].toMap();
        }

        source.commands = commands;
    }

    return data_sources;
}

daggycore::OptionalResult<Commands>
CJsonDataSourcesConvertor::getCommands
(
    const QVariantMap& commands_map
) const
{
    Commands commands;
    for (const QString& command_id : commands_map.keys()) {
        if (commands_map[command_id].type() != QVariant::Map)
            return
            {
                daggycore::DaggyErrors::ConvertError,
                QString("%1 command is not a map").arg(command_id).toStdString()
            };
        const QVariantMap& command_map = commands_map[command_id].toMap();
        for (const char* field : required_commands_field.keys()) {
            if (!command_map.contains(field)) {
                return
                {
                    daggycore::DaggyErrors::ConvertError,
                    QString("%1 command don't have required %2 field").arg(command_id, field).toStdString()
                };
            }
            if (command_map[field].type() != required_commands_field.value(field))
            {
                return
                {
                    daggycore::DaggyErrors::ConvertError,
                    QString("%1 command have incorrect type for %2 field").arg(command_id, field).toStdString()
                };
            }
        }
        Command command;
        command.id = command_id;
        command.exec = command_map[g_execField].toString();
        command.extension = command_map[g_extensionField].toString();
        if (command_map.contains(g_restartField) &&
            command_map[g_restartField].type() == QVariant::Bool)
        {
            command.restart = command_map[g_restartField].value<bool>();
        }
        commands[command.id] = command;
    }
    return commands;
}
