#ifndef CLOCALREMOTESERVER_H
#define CLOCALREMOTESERVER_H

#include "IRemoteServer.h"
#include "DataSource.h"

#include <QProcess>

class CLocalRemoteServer : public IRemoteServer
{
    Q_OBJECT
public:
    CLocalRemoteServer(const DataSource& data_source,
                       QObject* parent_pointer = nullptr);


    static constexpr const char* connection_type_global = "local";
    // IRemoteAgregator interface
protected:
    void startAgregator() override final;
    void stopAgregator() override final;

    // IRemoteServer interface
protected:
    void restartCommand(const QString& command_name) override final;
    void reconnect() override final;

private slots:
    void onProcessError(const QProcess::ProcessError error);
    void onProcessFinished(const int exit_code,
                           const QProcess::ExitStatus exit_status);

    QList<QProcess*> processes() const;
    QProcess* process(const QString& command_name) const;

    void onReadyReadStandardError();
    void onReadyReadStandardOutput();

    void onProcessStateChanged(const QProcess::ProcessState state);
};

#endif // CLOCALREMOTESERVER_H
