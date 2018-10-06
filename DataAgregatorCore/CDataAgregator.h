/*
Copyright 2017-2018 Mikhail Milovidov <milovidovmikhail@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef CDATAAGREGATOR_H
#define CDATAAGREGATOR_H

#include "dataagregatorcore_global.h"

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QMap>

#include "IRemoteAgregator.h"
#include "DataSource.h"

namespace QSsh {
class SshConnection;
}

namespace dataagregatorcore {

class IRemoteServersFabric;
class IRemoteAgregatorReciever;

class DATAAGREGATORCORESHARED_EXPORT CDataAgregator : public IRemoteAgregator
{
    Q_OBJECT
public:
    explicit CDataAgregator(const DataSources& data_sources,
                            IRemoteServersFabric* const remote_servers_fabric = nullptr,
                            QObject* const pParent = nullptr);
    virtual ~CDataAgregator() override;

    void connectRemoteAgregatorReciever(IRemoteAgregatorReciever* const remote_agregator_ptr);
    void dicsonnectRemoteAgregatorReciever(IRemoteAgregator* const remote_agregator_ptr);

    size_t runingRemoteCommandsCount() const override final;

private:
    void startAgregator() override final;
    void stopAgregator() override final;

    QList<IRemoteAgregator*> remoteAgregators() const;

    void createRemoteServer(const DataSource& data_source);

    IRemoteAgregator* getRemoteServer(const QString& server_name) const;
    bool isExistsRemoteServer(const QString& server_name) const;

    void closeConnections();

    size_t notStoppedRemoteAgregatorsCount() const;

    void onRemoteAgregatorStateChanged(const State agregator_state);

    const DataSources data_sources_;
    IRemoteServersFabric* const remote_servers_fabric_;

};
}

#endif // CDATAAGREGATOR_H
