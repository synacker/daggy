#ifndef IREMOTEAGREGATOR_H
#define IREMOTEAGREGATOR_H

#include <QObject>
#include "dataagregatorcore_global.h"

#include "RemoteCommand.h"

class DATAAGREGATORCORESHARED_EXPORT IRemoteAgregator : public QObject
{
    Q_OBJECT
public:
    enum class ConnectionStatus {
        NotConnected,
        Connected,
        Disconnected,
        ConnectionError
    };

    enum class State {
        Stopped,
        Run,
        Stopping
    };

    explicit IRemoteAgregator(QObject *pParent = nullptr);
    virtual ~IRemoteAgregator() = default;

    bool isExistsRunningRemoteCommands() const;

    virtual size_t runingRemoteCommandsCount() const = 0;

    bool isStoping() const;

    virtual void start();
    virtual void stop();

    State state() const;

signals:
    void connectionStatusChanged(QString serverId, ConnectionStatus status, QString message);
    void remoteCommandStatusChanged(QString serverId, QString commandName, RemoteCommand::Status status, int exitCode);
    void newRemoteCommandStream(QString serverId, RemoteCommand::Stream stream);

    void stateChanged(State state);

protected:
    virtual void startAgregator() = 0;
    virtual void stopAgregator() = 0;

    void setState(const State state);
private:

    State m_state;
    bool m_isStoping;
};

#endif // IREMOTEAGREGATOR_H
