/*
Copyright 2017-2020 Milovidov Mikhail

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "Precompiled.h"
#include "IDataProviderFabric.h"

#include "Common.h"

using namespace daggycore;

IDataProviderFabric::IDataProviderFabric
(
        QString type_arg,
        QObject* parent
)
    : QObject(parent)
    , type(std::move(type_arg))
{

}

OptionalResult<IDataProvider*> IDataProviderFabric::create
(
        const DataSource& data_source,
        QObject* parent
)
{
    if (data_source.type != type) {
        return
        {
            DaggyErrors::IncorrectProviderType,
            QString("Provider types dismatch: %1 and %2")
                    .arg(data_source.type)
                    .arg(type).toStdString()
        };
    }

    auto check_null_commands_result = checkNullCommands(data_source.commands);
    if (!check_null_commands_result) {
        return check_null_commands_result;
    }
    return createDataProvider(data_source, parent);
}

Result IDataProviderFabric::checkNullCommands(const Commands& commands) const
{
    Result result;
    QString error_message;
    for (const Command& command : commands) {
        if (command.isNull())
        {
            result =
            {
                DaggyErrors::NullCommand,
                QString("Command with id %1 is null").arg(command.id).toStdString()
            };
            break;
        }
    }
    return result;
}
