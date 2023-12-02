#include "../Precompiled.hpp"
#include "CSsh.hpp"
#include "../Errors.hpp"

namespace daggy {
namespace providers {

const QString CSsh::provider_type = "ssh";

CSsh::CSsh(const QString& session, QString host, Settings settings, sources::Commands commands, QObject *parent)
    : CLocal(session, std::move(commands), parent)
    , host_(std::move(host))
    , settings_(std::move(settings))
{

}

std::error_code CSsh::start() noexcept
{
    return errors::success;
}

std::error_code CSsh::stop() noexcept
{
    return errors::success;
}

const QString& CSsh::type() const noexcept
{
    return provider_type;
}

void CSsh::startProcess(QProcess* process, const QString& command)
{

}

}
}
