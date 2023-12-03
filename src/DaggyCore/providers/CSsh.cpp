#include "../Precompiled.hpp"
#include "CSsh.hpp"
#include "../Errors.hpp"

namespace daggy {
namespace providers {

const QString CSsh::provider_type = "ssh";

CSsh::CSsh(const QString& session,
           QString host,
           Settings settings,
           sources::Commands commands,
           QObject *parent)
    : CLocal(session, std::move(commands), parent)
    , host_(std::move(host))
    , settings_(std::move(settings))
    , ssh_master_(nullptr)
{

}

std::error_code CSsh::start() noexcept
{
    startMaster();
    return CLocal::start();
}

std::error_code CSsh::stop() noexcept
{
    auto result = CLocal::stop();
    stopMaster();
    return result;
}

const QString& CSsh::type() const noexcept
{
    return provider_type;
}

QProcess* CSsh::startProcess(const sources::Command& command)
{
    const auto& process_name = command.first;

    return CLocal::startProcess(process_name, "ssh", makeSlaveArguments(command));
}

void CSsh::terminate(QProcess* process)
{
    process->kill();
}

void CSsh::onMasterProcessError(QProcess::ProcessError error)
{
    switch (error) {
    case QProcess::FailedToStart:
    case QProcess::Crashed:
    case QProcess::Timedout:
    case QProcess::ReadError:
    case QProcess::WriteError:
    case QProcess::UnknownError:
        terminateAll();
        stopMaster();
        break;
    }
}

void CSsh::startMaster()
{
    if (!ssh_master_ && settings_.control.isEmpty())
    {
        ssh_master_ = new QProcess(this);

        ssh_master_->start("ssh", makeMasterArguments());
        ssh_master_->waitForStarted();
    }
}

void CSsh::stopMaster()
{
    if (ssh_master_) {
        ssh_master_->terminate();
        ssh_master_->waitForFinished();
        ssh_master_->deleteLater();
        ssh_master_ = nullptr;
    }
}

QStringList CSsh::makeMasterArguments() const
{
    QStringList result;
    if (!settings_.passphrase.isEmpty())
        result << "-p" << settings_.passphrase;

    result << "-F" << settings_.config << "-M" << host_;

    return {"-M", host_};
}

QStringList CSsh::makeSlaveArguments(const sources::Command& command) const
{
    QStringList result({"-F", settings_.config});
    if (!settings_.control.isEmpty())
        result << "-o" << QString("ControlPath=%1").arg(settings_.control);
    result << host_;
    QString exec = command.second.exec;
    const auto& parameters = command.second.getParameters();
    if (!parameters.isEmpty()) {
        for (const auto& parameter : parameters) {
            exec += " " + parameter;
        }
    }
    result << exec;
    return result;
}

}
}
