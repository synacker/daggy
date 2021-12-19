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
#include "CSsh2Fabric.hpp"

#include "CSsh2.hpp"
#include "ssh2/Ssh2Types.hpp"
#include "Result.hpp"

using namespace qtssh2;

namespace  {

constexpr const char* g_userField = "user";
constexpr const char* g_portField = "port";
constexpr const char* g_passphraseField = "passphrase";
constexpr const char* g_keyField = "key";
constexpr const char* g_keyphraseField = "keyphrase";
constexpr const char* g_knownHostsField = "knownhosts";
constexpr const char* g_timeoutField = "timeout";

constexpr const std::pair<const char*, QMetaType::Type> parameters_field[] =
{
    {g_userField, QMetaType::QString},
    {g_portField, QMetaType::UInt},
    {g_passphraseField, QMetaType::QString},
    {g_keyField, QMetaType::QString},
    {g_keyphraseField, QMetaType::QString},
    {g_knownHostsField, QMetaType::QString},
    {g_timeoutField, QMetaType::UInt}
};

daggy::Result<Ssh2Settings> convert(const QVariantMap& parameters)
{
    Ssh2Settings result;

    for (const auto& field : parameters_field) {
        if (parameters.contains(field.first) && parameters[field.first].metaType() != QMetaType(field.second))
            return {
                daggy::errors::make_error_code(DaggyErrorConvertion),
                QString("Parameters field '%1' has invalid type").arg(field.first)
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


}


daggy::providers::CSsh2Fabric::CSsh2Fabric()
{

}

const QString& daggy::providers::CSsh2Fabric::type() const
{
    return CSsh2::provider_type;
}

daggy::Result<daggy::providers::IProvider*> daggy::providers::CSsh2Fabric::createProvider(const Source& source,
                                                                                          QObject* parent)
{
    const auto parameters = convert(source.second.parameters);
    if (!parameters)
        return
        {
            parameters.error, parameters.message
        };

    const auto& properties = source.second;

    QHostAddress host;
    if (properties.host.isEmpty())
        host = QHostAddress::LocalHost;
    else
        host = QHostAddress(properties.host);

    return new CSsh2(std::move(host),
                     std::move(*parameters),
                     properties.commands,
                     parent);
}
