#ifndef IREMOTEAGREGATORRECIEVER_H
#define IREMOTEAGREGATORRECIEVER_H

#include <QObject>

#include "dataagregatorcore_global.h"
#include "RemoteCommand.h"
#include "RemoteConnectionStatus.h"

class DATAAGREGATORCORESHARED_EXPORT IRemoteAgregatorReciever : public QObject
{
    Q_OBJECT
public:
    IRemoteAgregatorReciever(QObject* const parent_ptr = nullptr);
    virtual ~IRemoteAgregatorReciever() = default;

public slots:
    virtual void onConnectionStatusChanged(const QString server_name,
                                           const RemoteConnectionStatus status,
                                           const QString message) = 0;
    virtual void onRemoteCommandStatusChanged(const QString server_name,
                                              const RemoteCommand remote_command,
                                              const RemoteCommand::Status status,
                                              const int exit_code) = 0;
    virtual void onNewRemoteCommandStream(const QString server_name,
                                          const RemoteCommand::Stream stream) = 0;


};

#endif // IREMOTEAGREGATORRECIEVER_H
