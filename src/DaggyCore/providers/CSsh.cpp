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

CSsh::~CSsh()
{
    if (ssh_master_)
    {
        ssh_master_->kill();
        ssh_master_->waitForFinished();
        ssh_master_.reset();
    }
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

const QString& CSsh::controlPath() const
{
    static const QString control_path = Settings::tempControlPath(session()) + "_" + host_;
    return settings_.control.isEmpty() ? control_path : settings_.control;
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
        ssh_master_.reset(new QProcess());

        ssh_master_->start("ssh", makeMasterArguments());
        ssh_master_->waitForStarted();
        ssh_master_->waitForReadyRead();
    }
}

void CSsh::stopMaster()
{
    if (ssh_master_) {
        QProcess::execute("ssh", {"-S", controlPath(), "-O", "exit", host_});
        ssh_master_->terminate();
        ssh_master_->waitForFinished();
        ssh_master_.reset();
    }
}

QStringList CSsh::makeMasterArguments() const
{
    QStringList result;
    if (!settings_.passphrase.isEmpty())
        result << "-p" << settings_.passphrase;

    result << "-tt" << controlArguments()  << "-F" << settings_.config << "-M" << host_;

    return result;
}

QStringList CSsh::makeSlaveArguments(const sources::Command& command) const
{
    QStringList result({"-F", settings_.config});
    result << controlArguments() << host_;

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

QStringList CSsh::controlArguments() const
{
    return { "-o", "ControlMaster=auto", "-o", QString("ControlPath=%1").arg(controlPath())};
}

const QString& CSsh::Settings::tempPath()
{
    static const QString temp_path = QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/.ssh/daggy";
    QDir temp_dir(temp_path);
    if (!temp_dir.exists())
        temp_dir.mkpath(".");
    return temp_path;
}

QString CSsh::Settings::tempConfigPath(const QString& session)
{
    return tempPath() + "/ssh_config_" + session;
}

QString CSsh::Settings::tempControlPath(const QString& session)
{
    return tempPath() + "/ssh_mux_" + session;
}

}
}
