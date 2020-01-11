/*
Copyright 2017-2020 Milovidov Mikhail

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#pragma once

#include "LastError.h"
#include "Common.h"
#include "DataSource.h"

#include <memory>

#include <QMap>
#include <QObject>

namespace daggycore {
class IDataProviderFabric;
class IDataAggregator;
class IDataProvider;

class DAGGYCORE_EXPORT DaggyCore : public QObject, public LastError
{
    Q_OBJECT
    Q_ENUMS(State)
    Q_PROPERTY(State state READ state NOTIFY stateChanged)
public:
    enum State {
        NotStarted,
        Started,
        Finishing,
        Finished
    };

    DaggyCore(const QList<IDataProviderFabric*>& fabrics,
              const QList<IDataAggregator*>& aggregators,
              const DataSources& data_sources,
              QObject* parent = nullptr);

    bool start();
    void stop();

    State state() const;

signals:
    void stateChanged(State state);

    void dataProviderStateChanged(const QString provider_id,
                                  const int state);
    void dataProviderError(const QString provider_id,
                           const std::error_code error_code);

    void commandStateChanged(const QString provider_id,
                             const QString command_id,
                             const Command::State state,
                             const int exit_code);
    void commandStream(const QString provider_id,
                       const QString command_id,
                       const Command::Stream stream);
    void commandError(const QString provider_id,
                      const QString command_id,
                      const std::error_code error_code);

private slots:
    void onDataProviderStateChanged(const int state);
    void onDataProviderError(const std::error_code error_code);

    void onCommandStateChanged(const QString command_id,
                               const Command::State state,
                               const int exit_code);
    void onCommandStream(const QString command_id,
                         const Command::Stream stream);
    void onCommandError(const QString command_id,
                        const std::error_code error_code);

private:
    bool initialize(const QList<IDataProviderFabric*>& fabrics,
                             const QList<IDataAggregator*>& aggregators,
                             const DataSources& data_sources);

    bool addDataProvidersFabric(IDataProviderFabric* new_fabric);
    bool addDataAggregator(IDataAggregator* aggregator);

    IDataProviderFabric* getFabric(const QString& type) const;
    QList<IDataAggregator*> getAggregators() const;
    QList<IDataProviderFabric*> getFabrics() const;
    QList<IDataProvider*> getProviders() const;
    IDataProvider* getProvider(const QString& provider_id) const;

    bool createProvider(const DataSource& data_source);

    void setState(State state);
    void destroyDaggyObjects();

    int activeDataProvidersCount() const;
    bool isActiveProvider(const IDataProvider* const provider) const;

    const DataSources data_sources_;
    State state_;
};

}
