#pragma once

#include "CLocal.hpp"
#include <QScopedPointer>

namespace daggy {
namespace providers {

class CSsh : public CLocal
{
    Q_OBJECT
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
    ~CSsh();

    std::error_code start() noexcept override;
    std::error_code stop() noexcept override;
    const QString& type() const noexcept override;

    QString controlPath() const;

    static const QString provider_type;

protected:
    QProcess* startProcess(const daggy::sources::Command& command) override;
    void terminate(QProcess* process) override;

private slots:
    void onMasterProcessError(QProcess::ProcessError error);

private:
    std::error_code startMaster();
    void stopMaster();

    QString masterErrorFile() const;

    QStringList makeMasterArguments() const;
    QStringList makeSlaveArguments(const sources::Command& command) const;

    QStringList controlArguments(bool master) const;
    void controlTerminate() const;

    const QString host_;
    const Settings settings_;

    QScopedPointer<QProcess, QScopedPointerDeleteLater> ssh_master_;
};

}
}


