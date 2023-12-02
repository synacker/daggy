#include "../Precompiled.hpp"
#include "CSshFabric.hpp"
#include "CSsh.hpp"

namespace {
const QString g_type = "ssh";

constexpr const char* g_configField = "config";
constexpr const char* g_passphraseField = "passphrase";

constexpr const std::pair<const char*, QMetaType::Type> parameters_field[] =
{
    {g_configField, QMetaType::QString},
    {g_passphraseField, QMetaType::QString},
};

daggy::Result<daggy::providers::CSsh::Settings> convert(const QVariantMap& parameters)
{
    daggy::providers::CSsh::Settings result;

    for (const auto& field : parameters_field) {
        if (parameters.contains(field.first) && parameters[field.first].metaType() != QMetaType(field.second))
            return {
                daggy::errors::make_error_code(DaggyErrorSourceConvertion),
                QString("Parameters field '%1' has invalid type").arg(field.first)
            };
    }

    if (parameters.contains(g_configField))
        result.ssh_config = parameters[g_configField].toString();
    else
        result.ssh_config = QDir::homePath() + "/.ssh/config";

    if (parameters.contains(g_passphraseField))
        result.passphrase = parameters[g_passphraseField].toString();

    return result;
}
}

namespace daggy {
namespace providers {

CSshFabric::CSshFabric()
{

}

const QString& CSshFabric::type() const
{
    return g_type;
}

daggy::Result<IProvider*> CSshFabric::createProvider(const QString& session, const Source& source, QObject* parent)
{
    const auto parameters = convert(source.second.parameters);
    if (!parameters) {
        return
            {
                parameters.error, parameters.message
            };
    }

    const auto& properties = source.second;

    return new CSsh(session,
                    properties.host,
                    std::move(*parameters),
                    properties.commands,
                    parent);
}
}
}




