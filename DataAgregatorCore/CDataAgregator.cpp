/*
Copyright 2017-2018 Milovidov Mikhail

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "Precompiled.h"
#include "CDataAgregator.h"

#include <ssh/sshconnection.h>

#include "CDefaultRemoteServersFabric.h"
#include "IRemoteAgregatorReciever.h"

CDataAgregator::CDataAgregator(const DataSources& data_sources,
                               IRemoteServersFabric* const remote_servers_fabric,
                               QObject* pParent)
    : IRemoteAgregator(pParent)
    , data_sources_(data_sources)
    , remote_servers_fabric_(remote_servers_fabric == nullptr ? new CDefaultRemoteServersFabric : remote_servers_fabric)
{
}

CDataAgregator::~CDataAgregator()
{
    delete remote_servers_fabric_;
}

void CDataAgregator::connectRemoteAgregatorReciever(IRemoteAgregatorReciever* const remote_agregator_ptr)
{
    connect(this, &IRemoteAgregator::connectionStatusChanged, remote_agregator_ptr, &IRemoteAgregatorReciever::onConnectionStatusChanged);
    connect(this, &IRemoteAgregator::remoteCommandStatusChanged, remote_agregator_ptr, &IRemoteAgregatorReciever::onRemoteCommandStatusChanged);
    connect(this, &IRemoteAgregator::newRemoteCommandStream, remote_agregator_ptr, &IRemoteAgregatorReciever::onNewRemoteCommandStream);
}

void CDataAgregator::dicsonnectRemoteAgregatorReciever(IRemoteAgregator* const remote_agregator_ptr)
{
    disconnect(remote_agregator_ptr);
}

void CDataAgregator::startAgregator()
{
    for (const DataSource& data_source : data_sources_) {
        if (!isExistsRemoteServer(data_source.server_name)) {
            createRemoteServer(data_source);
        }
    }

    for (IRemoteAgregator* const remote_server_ptr : remoteAgregators())
        remote_server_ptr->start();
}

void CDataAgregator::stopAgregator()
{
    for (IRemoteAgregator* const remote_server_ptr : remoteAgregators())
        remote_server_ptr->stop();
}

QList<IRemoteAgregator*> CDataAgregator::remoteAgregators() const
{
    return findChildren<IRemoteAgregator*>();
}

void CDataAgregator::createRemoteServer(const DataSource& data_source)
{
    IRemoteAgregator* const remote_server_ptr = remote_servers_fabric_->createRemoteServer(data_source, this);
    if (remote_server_ptr) {
        remote_server_ptr->setObjectName(data_source.server_name);

        connect(remote_server_ptr, &IRemoteAgregator::connectionStatusChanged, this, &IRemoteAgregator::connectionStatusChanged);
        connect(remote_server_ptr, &IRemoteAgregator::remoteCommandStatusChanged, this, &IRemoteAgregator::remoteCommandStatusChanged);
        connect(remote_server_ptr, &IRemoteAgregator::newRemoteCommandStream, this, &IRemoteAgregator::newRemoteCommandStream);

        connect(remote_server_ptr, &IRemoteAgregator::stateChanged, this, &CDataAgregator::onRemoteAgregatorStateChanged);
    }
}

IRemoteAgregator* CDataAgregator::getRemoteServer(const QString& server_name) const
{
    return findChild<IRemoteAgregator*>(server_name);
}

bool CDataAgregator::isExistsRemoteServer(const QString& server_name) const
{
    return getRemoteServer(server_name) != nullptr;
}

size_t CDataAgregator::notStoppedRemoteAgregatorsCount() const
{
    size_t result = 0;
    for (const IRemoteAgregator* remote_agregator_ptr : remoteAgregators()) {
        if (remote_agregator_ptr->state() != State::Stopped)
            result++;
    }
    return result;
}

void CDataAgregator::onRemoteAgregatorStateChanged(const IRemoteAgregator::State agregator_state)
{
    if (agregator_state == State::Stopped && notStoppedRemoteAgregatorsCount() == 0) {
        setStopped();
    }
}
