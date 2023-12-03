#include "../Precompiled.hpp"
#include "CSshFabric.hpp"
#include "CSsh.hpp"

namespace {
const QString g_type = "ssh";

const char* g_ssh_control_config = R"CONFIG(

Host *
  ControlMaster auto
  ControlPath %1/ssh_mux_%h_%p_%r_%2
)CONFIG";

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

std::error_code makeSessionSshConfig(QString& config_path, const QString& session)
{
    auto config_file = QFile(config_path);
    config_file.open(QIODevice::ReadOnly);
    QString ssh_config = config_file.readAll();
    config_file.close();
    ssh_config += QString(g_ssh_control_config).arg(QDir::tempPath(), session);

    config_path = QDir::tempPath() + QString("/ssh_mux_config_%1").arg(session);
    QSaveFile temp_ssh_config(config_path);
    if (!temp_ssh_config.open(QIODevice::WriteOnly))
        return daggy::errors::make_error_code(DaggyErrorInternal);

    temp_ssh_config.write(qPrintable(ssh_config));
    if (!temp_ssh_config.commit())
        return daggy::errors::make_error_code(DaggyErrorInternal);

    return daggy::errors::success;
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

    auto error = makeSessionSshConfig(parameters->ssh_config, session);
    if (error)
        return {error, "Cannot create ssh session config"};

    const auto& properties = source.second;

    return new CSsh(session,
                    properties.host,
                    std::move(*parameters),
                    properties.commands,
                    parent);
}
}
}




