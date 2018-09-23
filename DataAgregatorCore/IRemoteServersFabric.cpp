#include "Precompiled.h"
#include "IRemoteServersFabric.h"

IRemoteServersFabric::IRemoteServersFabric(const QString& connectionType)
    : m_connectionType(connectionType)
{

}

QString IRemoteServersFabric::connectionType() const
{
    return m_connectionType;
}
