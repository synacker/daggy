/*
MIT License

Copyright (c) 2020 Mikhail Milovidov

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#pragma once

#include "Common.h"
#include "DataSource.h"

#include <memory>

#include <QMap>
#include <QObject>

#include "IDataProvider.h"

#include "OptionalResult.h"

namespace daggyconv {
class IDataSourceConvertor;
}

namespace daggycore {
class IDataProviderFabric;
class IDataAggregator;
class IDataProvider;

class DAGGYCORE_EXPORT DaggyCore : public QObject
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

    DaggyCore(DataSources data_sources,
              QObject* parent = nullptr);
    DaggyCore(QObject* parent = nullptr);

    ~DaggyCore();

    void setDataSources(DataSources data_sources);
    Result setDataSources(
        const QString& data_sources_text,
        const QString& text_format_type
    );

    const DataSources& dataSources() const;

    Result start();
    void stop();

    State state() const;

    Result addDataProvidersFabric(std::unique_ptr<IDataProviderFabric> new_fabric);
    Result addDataAggregator(IDataAggregator* aggregator);
    Result addDataSourceConvertor(std::unique_ptr<daggyconv::IDataSourceConvertor> convertor);

signals:
    void stateChanged(State state);

    void dataProviderStateChanged(QString provider_id,
                                  IDataProvider::State state);
    void dataProviderError(QString provider_id,
                           std::error_code error_code);

    void commandStateChanged(QString provider_id,
                             QString command_id,
                             Command::State state,
                             int exit_code);
    void commandStream(QString provider_id,
                       QString command_id,
                       Command::Stream stream);
    void commandError(QString provider_id,
                      QString command_id,
                      std::error_code error_code);

private slots:
    void onDataProviderStateChanged(IDataProvider::State state);
    void onDataProviderError(std::error_code error_code);

    void onCommandStateChanged(QString command_id,
                               Command::State state,
                               int exit_code);
    void onCommandStream(QString command_id,
                         Command::Stream stream);
    void onCommandError(QString command_id,
                        std::error_code error_code);

private:

    IDataProviderFabric* getFabric(const QString& type) const;
    daggyconv::IDataSourceConvertor* getConvertor(const QString& type) const;

    QList<IDataAggregator*> getAggregators() const;
    QList<IDataProvider*> getProviders() const;
    IDataProvider* getProvider(const QString& provider_id) const;

    daggycore::Result createProvider(const DataSource& data_source);

    void setState(State state);

    int activeDataProvidersCount() const;
    bool isActiveProvider(const IDataProvider* const provider) const;

    std::unordered_map<QString, std::unique_ptr<IDataProviderFabric>> data_provider_fabrics_;
    std::unordered_map<QString, std::unique_ptr<daggyconv::IDataSourceConvertor>> data_convertors_;

    DataSources data_sources_;
    State state_;
};

}
