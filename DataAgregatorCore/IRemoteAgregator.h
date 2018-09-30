#ifndef IREMOTEAGREGATOR_H
#define IREMOTEAGREGATOR_H

#include <QObject>
#include "dataagregatorcore_global.h"

#include "RemoteCommand.h"
#include "RemoteConnectionStatus.h"

class DATAAGREGATORCORESHARED_EXPORT IRemoteAgregator : public QObject
{
    Q_OBJECT
public:
    enum class State {
        Stopped,
        Run,
        Stopping
    };

    explicit IRemoteAgregator(QObject *pParent = nullptr);
    virtual ~IRemoteAgregator() = default;

    bool isExistsRunningRemoteCommands() const;

    virtual size_t runingRemoteCommandsCount() const = 0;

    void start();
    void stop();

    State state() const;

signals:
    void connectionStatusChanged(QString server_name,
                                 RemoteConnectionStatus status,
                                 QString message);
    void remoteCommandStatusChanged(QString server_name,
                                    RemoteCommand remote_command,
                                    RemoteCommand::Status status,
                                    int exit_code);
    void newRemoteCommandStream(QString server_id, RemoteCommand::Stream stream);

    void stateChanged(State state);

protected:
    virtual void startAgregator() = 0;
    virtual void stopAgregator() = 0;

    void setStopped();

private:
    void setState(const State state);
    State state_;
    size_t running_remote_command_count_;
};

#endif // IREMOTEAGREGATOR_H
