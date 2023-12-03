#pragma once

#include "CLocal.hpp"

namespace daggy {
namespace providers {

class CSsh : public CLocal
{
public:
    struct Settings
    {
        QString config;
        QString passphrase;
        QString control;
    };

    CSsh(const QString& session,
         QString host,
         Settings settings,
         sources::Commands commands,
         QObject *parent = nullptr);

    std::error_code start() noexcept override;
    std::error_code stop() noexcept override;
    const QString& type() const noexcept override;

    static const QString provider_type;

protected:
    QProcess* startProcess(const daggy::sources::Command& command) override;

private slots:
    void onMasterProcessError(QProcess::ProcessError error);

private:
    void startMaster();
    void stopMaster();

    QStringList makeMasterArguments() const;
    QStringList makeSlaveArguments(const sources::Command& command) const;

    const QString host_;
    const Settings settings_;

    QProcess* ssh_master_;
};

}
}


