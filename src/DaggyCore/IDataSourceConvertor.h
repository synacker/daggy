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
#pragma once

#include <QObject>

#include "DataSource.h"
#include "OptionalResult.h"

namespace daggyconv {

class DAGGYCORE_EXPORT IDataSourceConvertor : public QObject
{
    Q_OBJECT
public:
    IDataSourceConvertor
    (
        QString type_arg,
        QObject* parent = nullptr
    );

    virtual ~IDataSourceConvertor() = default;
    virtual daggycore::OptionalResult<daggycore::DataSources> convert
    (
            const QString& data
    ) const = 0;

    const QString type;

    constexpr static const char* g_sourcesField = "sources";

    constexpr static const char* g_hostField = "host";
    constexpr static const char* g_typeField = "type";
    constexpr static const char* g_parametersField = "parameters";
    constexpr static const char* g_commandsField = "commands";
    constexpr static const char* g_reconnectField = "reconnect";

    static const QMap<const char*, QVariant::Type> required_source_field;

    constexpr static const char* g_execField = "exec";
    constexpr static const char* g_extensionField = "extension";
    constexpr static const char* g_restartField = "restart";

    static const QMap<const char*, QVariant::Type> required_commands_field;
};
}
