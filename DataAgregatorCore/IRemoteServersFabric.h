#ifndef ISERVERCONNECTIONFABRIC_H
#define ISERVERCONNECTIONFABRIC_H

#include "DataSource.h"

class IRemoteAgregator;

class IRemoteServersFabric
{
public:
    virtual ~IRemoteServersFabric() = default;

    virtual IRemoteAgregator* createRemoteServer(const DataSource& data_source,
                                                 QObject* pParent) = 0;

};

#endif // ISERVERCONNECTIONFABRIC_H
