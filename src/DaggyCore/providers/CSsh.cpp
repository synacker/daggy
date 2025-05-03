#include "CSsh.hpp"
#include "../Errors.hpp"

namespace {
std::chrono::milliseconds check_master_connection_timeout(100);
}

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
#ifndef Q_OS_WIN
    return startMaster();
#else
    return CLocal::start();
#endif
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

QString CSsh::controlPath() const
{
    const uint control_hash = qHash(session() + host_);
    const QString control_path = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + QString("/%1.socket").arg(control_hash);
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

std::error_code CSsh::startMaster()
{
    if (!ssh_master_ && settings_.control.isEmpty())
    {
        ssh_master_.reset(new QProcess());

        ssh_master_->setStandardErrorFile(masterErrorFile(), QIODeviceBase::Append);
        ssh_master_->start("ssh", makeMasterArguments());
        ssh_master_->waitForStarted();
        if (ssh_master_->state() != QProcess::Running) {
            return daggy::errors::make_error_code(DaggyErrorProviderFailedToStart);
        }

        auto* timer = new QTimer(this);
        connect(timer, &QTimer::timeout, [timer, this](){
            QProcess check_process;
            check_process.setStandardErrorFile(masterErrorFile(), QIODeviceBase::Append);
            check_process.start("ssh", {"-o", QString("ControlPath=%1").arg(controlPath()), "-O", "check", host_});
            check_process.waitForFinished();

            const int failed_check = 255;
            auto code = check_process.exitCode();
            if (code != failed_check) {
                QProcess check_connection;
                check_connection.setStandardErrorFile(masterErrorFile(), QIODeviceBase::Append);
                QStringList check_arguments = controlArguments(false);
                check_arguments += {"-q", host_, "exit"};
                check_connection.start("ssh", check_arguments);
                check_connection.waitForFinished();
                if (check_connection.exitCode() == 0) {
                    CLocal::start();
                    timer->stop();
                    timer->deleteLater();
                }
            }
        });
        timer->start(check_master_connection_timeout);
    }
    return errors::success;
}

void CSsh::stopMaster()
{
    if (ssh_master_) {
        if (QFile::exists(controlPath())) {
            QProcess terminate_process;
            terminate_process.setStandardErrorFile(masterErrorFile(), QIODeviceBase::Append);
            terminate_process.start("ssh", {"-S", controlPath(), "-O", "exit", host_});
            terminate_process.waitForFinished();
        }
        ssh_master_->terminate();
        ssh_master_->waitForFinished();
        ssh_master_.reset();
    }
}

QString CSsh::masterErrorFile() const
{
    return controlPath() + ".log";
}

QStringList CSsh::makeMasterArguments() const
{
    QStringList result;
    if (!settings_.passphrase.isEmpty())
        result << "-p" << settings_.passphrase;

    result << "-tt" << controlArguments(true)  << "-F" << settings_.config << "-M" << host_;

    return result;
}

QStringList CSsh::makeSlaveArguments(const sources::Command& command) const
{
    QStringList result({"-F", settings_.config});
    result << controlArguments(false) << host_;

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

QStringList CSsh::controlArguments(bool master) const
{
    if (!ssh_master_ && settings_.control.isEmpty())
        return {};

    return (master ?
            QStringList{ "-o", "ControlMaster=auto", "-o", QString("ControlPath=%1").arg(controlPath())} :
            QStringList{ "-o", "ControlMaster=no", "-S", controlPath()});
}

}
}
