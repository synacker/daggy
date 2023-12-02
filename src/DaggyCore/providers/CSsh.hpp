#pragma once

#include "CLocal.hpp"

namespace daggy {
namespace providers {

class CSsh : public CLocal
{
public:
    struct Settings
    {
        QString ssh_config;
        QString passphrase;
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
    void startProcess(QProcess* process, const QString& command) override;

private:
    const QString host_;
    const Settings settings_;
};

}
}


