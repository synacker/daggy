#include "../Precompiled.hpp"
#include "CSshFabric.hpp"
#include "CSsh.hpp"

namespace {
const QString g_type = "ssh";

const char* g_ssh_control_config = R"CONFIG(

Host *
  ControlMaster auto
  ControlPath %1
)CONFIG";

constexpr const char* g_configField = "config";
constexpr const char* g_passphraseField = "passphrase";
constexpr const char* g_controlField = "control";

constexpr const std::pair<const char*, QMetaType::Type> parameters_field[] =
{
    {g_configField, QMetaType::QString},
    {g_passphraseField, QMetaType::QString},
    {g_controlField, QMetaType::QString},
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
        result.config = parameters[g_configField].toString();
    else
        result.config = QDir::homePath() + "/.ssh/config";

    if (parameters.contains(g_passphraseField))
        result.passphrase = parameters[g_passphraseField].toString();

    if (parameters.contains(g_controlField))
        result.control = parameters[g_controlField].toString();

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
    auto parameters = convert(source.second.parameters);
    if (!parameters) {
        return
            {
                parameters.error, parameters.message
            };
    }

    const auto& properties = source.second;
    QString host = properties.host;
    if (host.isEmpty())
        host = source.first;

    return new CSsh(session,
                    host,
                    std::move(*parameters),
                    properties.commands,
                    parent);
}
}
}




