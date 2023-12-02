#pragma once

#include "IFabric.hpp"

namespace daggy
{
namespace providers
{

class CSshFabric : public IFabric
{
public:
    CSshFabric();

    const QString& type() const;

protected:
    daggy::Result<IProvider*> createProvider(const QString& session, const Source& source, QObject* parent);
};

}
}


