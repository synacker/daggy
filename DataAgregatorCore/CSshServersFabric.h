#ifndef CSSHSERVERSFABRIC_H
#define CSSHSERVERSFABRIC_H

#include "IRemoteServersFabric.h"

class CSshServersFabric : public IRemoteServersFabric
{
public:
    CSshServersFabric();

    IRemoteServer* createRemoteServer(const QString& serverName,
                                      const std::vector<RemoteCommand>& remoteCommands,
                                      const QVariantMap& connectionParameters,
                                      QObject* pParent) override;
};

#endif // CSSHSERVERSFABRIC_H
