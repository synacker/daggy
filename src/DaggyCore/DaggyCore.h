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

    ~DaggyCore() = default;

    template<typename Fabric, typename... Args> Result createProviderFabric(Args... args) {
        if (getFabric(Fabric::fabric_type))
            return Result{DaggyErrors::ProviderTypeAlreadyExists};
        return addDataProvidersFabric(new Fabric(args...));
    }

    template<typename Aggregator, typename... Args> Result createDataAggregator(Args... args) {
        return addDataAggregator(new Aggregator(args...));
    }

    template<typename Convertor, typename... Args> Result createConvertor(Args... args) {
        if (getConvertor(Convertor::convertor_type))
            return {DaggyErrors::ConvertorTypeAlreadyExists};
        return addDataSourceConvertor(new Convertor(args...));
    }

    void setDataSources(DataSources data_sources);
    Result setDataSources(
        const QString& data_sources_text,
        const QString& text_format_type
    );

    Result start();
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
    Result addDataProvidersFabric(IDataProviderFabric* new_fabric);
    Result addDataAggregator(IDataAggregator* aggregator);
    Result addDataSourceConvertor(daggyconv::IDataSourceConvertor* convertor);

    IDataProviderFabric* getFabric(const QString& type) const;
    daggyconv::IDataSourceConvertor* getConvertor(const QString& type) const;

    QList<IDataAggregator*> getAggregators() const;
    QList<IDataProviderFabric*> getFabrics() const;
    QList<IDataProvider*> getProviders() const;
    QList<daggyconv::IDataSourceConvertor*> getConvertors() const;
    IDataProvider* getProvider(const QString& provider_id) const;

    daggycore::Result createProvider(const DataSource& data_source);

    void setState(State state);

    int activeDataProvidersCount() const;
    bool isActiveProvider(const IDataProvider* const provider) const;

    DataSources data_sources_;
    State state_;
};

}
