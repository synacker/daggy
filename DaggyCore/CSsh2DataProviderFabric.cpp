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
constexpr const char* port_field = "port";
constexpr const char* user_field = "user";
constexpr const char* password_field = "password";
constexpr const char* key_field = "key";
constexpr const char* known_hosts_field = "known_hosts";
constexpr const char* timeout_field = "timeout";

const QMap<QString, QVariant::Type> connection_fields =
{
    {port_field, QVariant::Int},
    {user_field, QVariant::String},
    {password_field, QVariant::String},
    {key_field, QVariant::String},
    {known_hosts_field, QVariant::String},
    {timeout_field, QVariant::Int}
};

class VariantSettor
{
public:
    VariantSettor(const QVariantMap& map)
        : map_(map){};

    template<typename T> bool setIfExists(const QString& key, T& value) {
        return setIfExists(key, value, value);
    }

    template<typename T> bool setIfExists(const QString& key, T& value, const T& default_value) {
        if (!map_.contains(key)) {
            value = default_value;
            return false;
        }
        const QVariant& map_value = map_[key];
        if (!map_value.canConvert<T>()) {
            value = default_value;
            return false;
        }
        value = map_value.value<T>();
        return true;
    }

private:
    const QVariantMap& map_;
};

}

IDataProvider* CSsh2DataProviderFabric::createDataProvider(const DataSource& data_source, QObject* parent)
{
    Ssh2Settings ssh2_settings;
    VariantSettor settor(data_source.connection);
    settor.setIfExists(user_field, ssh2_settings.user);
    if (settor.setIfExists(password_field, ssh2_settings.passphrase, QString()))
        settor.setIfExists(key_field, ssh2_settings.key, QString());
    else
        settor.setIfExists(key_field, ssh2_settings.key);
    settor.setIfExists(known_hosts_field, ssh2_settings.known_hosts);
    settor.setIfExists(timeout_field, ssh2_settings.timeout);

    return new CSsh2DataProvider(QHostAddress(data_source.host),
                                 ssh2_settings,
                                 data_source.commands,
                                 parent);
}

QString CSsh2DataProviderFabric::type() const
{
    return IDataProvider::provider_type;
}

const QMap<QString, QVariant::Type>& CSsh2DataProviderFabric::connectionFields() const
{
    return connection_fields;
}
