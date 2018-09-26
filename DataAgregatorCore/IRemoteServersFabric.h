#ifndef ISERVERCONNECTIONFABRIC_H
#define ISERVERCONNECTIONFABRIC_H

#include "dataagregatorcore_global.h"
#include "IRemoteServer.h"

class DATAAGREGATORCORESHARED_EXPORT IRemoteServersFabric
{
public:
    IRemoteServersFabric(const QString& connectionType);
    virtual ~IRemoteServersFabric() = default;

    virtual IRemoteServer* createRemoteServer(const QString& serverName,
                                              const std::vector<RemoteCommand>& remoteCommands,
                                              const QVariantMap& connectionParameters,
                                              QObject* pParent) = 0;
    QString connectionType() const;

private:
    const QString m_connectionType;

};

#endif // ISERVERCONNECTIONFABRIC_H
