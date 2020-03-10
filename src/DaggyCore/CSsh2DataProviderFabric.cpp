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
constexpr const char* g_userField = "user";
constexpr const char* g_portField = "port";
constexpr const char* g_passphraseField = "passphrase";
constexpr const char* g_keyField = "key";
constexpr const char* g_keyphraseField = "keyphrase";
constexpr const char* g_knownHostsField = "knownhosts";
constexpr const char* g_timeoutField = "timeout";

constexpr const std::pair<const char*, QVariant::Type> parameters_field[] =
{
    {g_userField, QVariant::String},
    {g_portField, QVariant::UInt},
    {g_passphraseField, QVariant::String},
    {g_keyField, QVariant::String},
    {g_keyphraseField, QVariant::String},
    {g_knownHostsField, QVariant::String},
    {g_timeoutField, QVariant::UInt}
};

}

const char* CSsh2DataProviderFabric::fabric_type = CSsh2DataProvider::provider_type;

CSsh2DataProviderFabric::CSsh2DataProviderFabric(QObject* parent)
    : IDataProviderFabric(fabric_type, parent)
{

}

OptionalResult<IDataProvider*> CSsh2DataProviderFabric::createDataProvider(const DataSource& data_source, QObject* parent)
{
    const auto parameters = convertParameters(data_source.parameters);
    if (!parameters) {
        return {DaggyErrors::WrongSourceParameter};
    }
    QHostAddress host;
    if (data_source.host.isEmpty())
        host = QHostAddress::LocalHost;
    else
        host = QHostAddress(data_source.host);

    return new CSsh2DataProvider(host,
                                 parameters.value(),
                                 data_source.commands,
                                 parent);
}

OptionalResult<Ssh2Settings> CSsh2DataProviderFabric::convertParameters(const QVariantMap& parameters) const
{
    Ssh2Settings result;

    for (const auto& field : parameters_field) {
        if (parameters.contains(field.first) && parameters[field.first].type() != field.second)
            return Result
            {
                DaggyErrors::ConvertError,
                "Invalid parameters type"
            };
    }

    if (parameters.contains(g_userField))
        result.user = parameters[g_userField].toString();

    if (parameters.contains(g_portField))
        result.port = parameters[g_portField].toUInt();

    if (parameters.contains(g_passphraseField))
        result.passphrase = parameters[g_passphraseField].toString();

    if (parameters.contains(g_keyField))
        result.key = parameters[g_keyField].toString();

    if (parameters.contains(g_keyphraseField))
        result.keyphrase = parameters[g_keyphraseField].toString();

    if (parameters.contains(g_knownHostsField))
        result.known_hosts = parameters[g_knownHostsField].toString();

    if (parameters.contains(g_timeoutField))
        result.timeout = parameters[g_timeoutField].toUInt();

    return result;
}
