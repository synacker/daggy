#include "Precompiled.h"
#include "CSshServersFabric.h"

#include "CSshRemoteServer.h"

CSshServersFabric::CSshServersFabric()
    : IRemoteServersFabric(CSshRemoteServer::g_connectionType)
{

}

IRemoteServer* CSshServersFabric::createRemoteServer(const QString& serverName,
                                                     const std::vector<RemoteCommand>& remoteCommands,
                                                     const QVariantMap& connectionParameters,
                                                     QObject* pParent)
{
    return new CSshRemoteServer(serverName,
                                remoteCommands,
                                connectionParameters,
                                pParent);
}
