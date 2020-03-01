/*
Copyright 2017-2020 Milovidov Mikhail

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#pragma once

#include <QObject>

#include "daggycore_export.h"
#include "DataSource.h"
#include "Common.h"
#include "OptionalResult.h"

namespace daggycore {
class IDataProvider;

class DAGGYCORE_EXPORT IDataProviderFabric : public QObject
{
    Q_OBJECT
public:
    IDataProviderFabric(QString type, QObject* parent = nullptr);
    virtual ~IDataProviderFabric() = default;

    OptionalResult<IDataProvider*> create
    (
            const DataSource& data_source,
            QObject* parent
    );

    QString type() const;

protected:
    virtual OptionalResult<IDataProvider*> createDataProvider(const DataSource& data_source, QObject* parent) = 0;

private:
    Result checkNullCommands(const Commands& commands) const;

    QString type_;
};

}
