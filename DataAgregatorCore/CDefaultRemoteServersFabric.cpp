#include "Precompiled.h"
#include "CDefaultRemoteServersFabric.h"

#include "CSshRemoteServer.h"
#include "CLocalRemoteServer.h"

IRemoteAgregator* CDefaultRemoteServersFabric::createRemoteServer(const DataSource& data_source, QObject* parent_ptr)
{
    IRemoteServer* result = nullptr;

    if (data_source.connection_type.isEmpty() || data_source.connection_type == CSshRemoteServer::connection_type_global)
        result = new CSshRemoteServer(data_source, parent_ptr);
    else if (data_source.connection_type == CLocalRemoteServer::connection_type_global)
        result = new CLocalRemoteServer(data_source, parent_ptr);

    return result;
}
