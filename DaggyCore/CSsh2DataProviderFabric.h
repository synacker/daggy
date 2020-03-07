/*
Copyright 2017-2020 Milovidov Mikhail

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#pragma once
#include "daggycore_export.h"
#include "IDataProviderFabric.h"

namespace daggyssh2 {
struct Ssh2Settings;
}

namespace daggycore {
class IDataProvider;

class DAGGYCORE_EXPORT CSsh2DataProviderFabric : public IDataProviderFabric
{
    Q_OBJECT
public:
    CSsh2DataProviderFabric(QObject* parent = nullptr);

    constexpr static const char* fabric_type = "ssh2";
protected:
    OptionalResult<IDataProvider*> createDataProvider
    (
            const DataSource& data_source,
            QObject* parent
    ) override;

    OptionalResult<daggyssh2::Ssh2Settings> convertParameters(const QVariantMap& parameters) const;
};

}
