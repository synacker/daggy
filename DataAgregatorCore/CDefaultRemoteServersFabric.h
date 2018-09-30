#ifndef CDEFAULTREMOTESERVERSFABRIC_H
#define CDEFAULTREMOTESERVERSFABRIC_H

#include "IRemoteServersFabric.h"

class CDefaultRemoteServersFabric : public IRemoteServersFabric
{
public:
    CDefaultRemoteServersFabric() = default;

    IRemoteAgregator* createRemoteServer(const DataSource& data_source,
                                         QObject* parent_ptr) override;
};

#endif // CDEFAULTREMOTESERVERSFABRIC_H
