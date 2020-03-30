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
#include "IDataSourceConvertor.h"

#include "Common.h"

using namespace daggycore;
using namespace daggyconv;

const QMap<const char*, QVariant::Type> IDataSourceConvertor::required_source_field =
{
    {IDataSourceConvertor::g_typeField, QVariant::String},
    {IDataSourceConvertor::g_commandsField, QVariant::Map}
};

const QMap<const char*, QVariant::Type> IDataSourceConvertor::required_commands_field =
{
    {IDataSourceConvertor::g_execField, QVariant::String},
    {IDataSourceConvertor::g_extensionField, QVariant::String}
};

IDataSourceConvertor::IDataSourceConvertor(QString type_arg, QObject* parent)
    : QObject(parent)
    , type(std::move(type_arg))
{
    
}

daggycore::OptionalResult<Commands> IDataSourceConvertor::getCommands(const QVariantMap& commands_map)
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
