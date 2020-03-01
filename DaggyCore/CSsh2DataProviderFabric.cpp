/*
Copyright 2017-2020 Milovidov Mikhail

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "Precompiled.h"
#include "CSsh2DataProviderFabric.h"
#include "IDataProvider.h"
#include "Ssh2Types.h"
#include "CSsh2DataProvider.h"

using namespace daggycore;
using namespace daggyssh2;

namespace  {
constexpr const char* provider_type = "ssh2";
}

OptionalResult<IDataProvider*> CSsh2DataProviderFabric::createDataProvider(const DataSource& data_source, QObject* parent)
{
    if (!data_source.parameters.canConvert<Ssh2Settings>()) {
        return {DaggyErrors::WrongConnectionParameter};
    }

    const Ssh2Settings ssh2_settings = data_source.parameters.value<Ssh2Settings>();
    return new CSsh2DataProvider(QHostAddress(data_source.host),
                                 ssh2_settings,
                                 data_source.commands,
                                 parent);
}

QString CSsh2DataProviderFabric::type() const
{
    return provider_type;
}
