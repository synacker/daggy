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
#include "Precompiled.h"
#include "DaggyCore.h"

#include "IDataProviderFabric.h"
#include "IDataAggregator.h"
#include "IDataProvider.h"
#include "IDataSourceConvertor.h"

#include "CLocalDataProvidersFabric.h"

#ifdef SSH2_SUPPORT
#include "CSsh2DataProviderFabric.h"
#endif

#include "CJsonDataSourcesConvertor.h"

#ifdef YAML_SUPPORT
#include "CYamlDataSourcesConvertor.h"
#endif

using namespace daggycore;

DaggyCore::DaggyCore(DataSources data_sources,
                     QObject* parent)
    : QObject(parent)
    , data_sources_(std::move(data_sources))
    , state_(NotStarted)
{
    addDataProvidersFabric(std::make_unique<CLocalDataProvidersFabric>());
#ifdef SSH2_SUPPORT
    addDataProvidersFabric(std::make_unique<CSsh2DataProviderFabric>());
#endif

    data_convertors_[daggyconv::CJsonDataSourcesConvertor::convertor_type] = std::make_unique<daggyconv::CJsonDataSourcesConvertor>();
#ifdef YAML_SUPPORT
    data_convertors_[daggyconv::CYamlDataSourcesConvertor::convertor_type] = std::make_unique<daggyconv::CYamlDataSourcesConvertor>();
#endif
}

DaggyCore::DaggyCore(QObject* parent)
    : DaggyCore({}, parent)
{

}

DaggyCore::~DaggyCore()
{

}

DaggyCore::Version DaggyCore::version() const
{
    return
    {
        DAGGY_VERSION_FULL,
        DAGGY_VERSION_MAJOR,
        DAGGY_VERSION_MINOR,
        DAGGY_VERSION_PATCH,
        DAGGY_VERSION_BUILD,
        DAGGY_VERSION_POSTFIX,
        DAGGY_VENDOR
    };
}

void DaggyCore::setDataSources(DataSources data_sources)
{
    data_sources_ = std::move(data_sources);
}

Result DaggyCore::setDataSources
(
        const QString& data_sources_text,
        const QString& text_format_type
)
{
    const auto convertor = getConvertor(text_format_type);
    if (!convertor)
        return
        {
            DaggyErrors::ConvertError,
            QString("%1 convertion type is not supported").arg(text_format_type).toStdString()
        };
    const auto convertion = convertor->convert(data_sources_text);
    if (!convertion)
        return convertion.result();

    setDataSources(convertion.value());
    return Result::success;
}

const DataSources& DaggyCore::dataSources() const
{
    return data_sources_;
}

int DaggyCore::activeDataProvidersCount() const
{
    int result = 0;
    for (IDataProvider* provider : getProviders())
        if (isActiveProvider(provider))
            result++;
    return result;
}

bool DaggyCore::isActiveProvider(const IDataProvider* const provider) const
{
    bool result = false;
    switch (provider->state()) {
    case IDataProvider::NotStarted:
    case IDataProvider::FailedToStart:
    case IDataProvider::Finished:
        result = false;
        break;
    default:
        result = true;
    }
    return result;
}

Result DaggyCore::start()
{
    for (auto data_aggreagator : getAggregators()) {
        const auto result = data_aggreagator->prepare();
        if (!result)
            return result;
    }

    auto data_providers = getProviders();
    if (data_providers.empty()) {
        for (const auto& data_source : qAsConst(data_sources_)) {
            auto result = createProvider(data_source);
            if (!result)
                return result;
        }
    }
    data_providers = getProviders();
    if (data_providers.empty()) {
        setState(Finished);
        return Result::success;
    }

    setState(Started);
    for (IDataProvider* provider : data_providers) {
        provider->start();
    }

    return Result::success;
}

void DaggyCore::stop()
{
    if (state_ != Started) {
        return;
    }

    const auto data_providers = getProviders();
    if (data_providers.empty() || activeDataProvidersCount() == 0) {
        setState(Finished);
        return;
    }

    setState(Finishing);
    for (IDataProvider* provider : data_providers)
        provider->stop();
}



DaggyCore::State DaggyCore::state() const
{
    return state_;
}

void DaggyCore::onDataProviderStateChanged(const IDataProvider::State state)
{
    const QString& provider_id = sender()->objectName();
    emit dataProviderStateChanged(provider_id,
                                  state);

    const DataSource& data_source = data_sources_[provider_id];
    if (state == IDataProvider::Finished &&
        data_source.reconnect &&
        state_ == Started)
    {
        IDataProvider* provider = getProvider(provider_id);
        provider->start();
    }

    if (activeDataProvidersCount() == 0) {
        for (auto data_aggregator : getAggregators())
            data_aggregator->free();
        setState(Finished);
    }
}

void DaggyCore::onDataProviderError(std::error_code error_code)
{
    emit dataProviderError(sender()->objectName(),
                           error_code);
}

void DaggyCore::onCommandStateChanged(QString command_id,
                                      Command::State state,
                                      int exit_code)
{
    emit commandStateChanged(sender()->objectName(),
                             command_id,
                             state,
                             exit_code);
}

void DaggyCore::onCommandStream(QString command_id,
                                Command::Stream stream)
{
    emit commandStream(sender()->objectName(),
                       command_id,
                       stream);
}

void DaggyCore::onCommandError(QString command_id,
                               std::error_code error_code)
{
    emit commandError(sender()->objectName(),
                      command_id,
                      error_code);
}

IDataProviderFabric* DaggyCore::getFabric(const QString& type) const
{
    IDataProviderFabric* result = nullptr;
    if (data_provider_fabrics_.find(type) != data_provider_fabrics_.end())
        result = data_provider_fabrics_.at(type).get();
    return result;
}

daggyconv::IDataSourceConvertor* DaggyCore::getConvertor(const QString& type) const
{
    daggyconv::IDataSourceConvertor* result = nullptr;
    if (data_convertors_.find(type) != data_convertors_.end())
        result = data_convertors_.at(type).get();
    return result;
}

QList<IDataAggregator*> DaggyCore::getAggregators() const
{
    return findChildren<IDataAggregator*>();
}

QList<IDataProvider*> DaggyCore::getProviders() const
{
    return findChildren<IDataProvider*>();
}

IDataProvider* DaggyCore::getProvider(const QString& provider_id) const
{
    IDataProvider* result = nullptr;
    if (!provider_id.isEmpty())
        result = findChild<IDataProvider*>(provider_id);
    return result;
}

Result DaggyCore::createProvider(const DataSource& data_source)
{
    IDataProviderFabric* fabric = getFabric(data_source.type);
    if (fabric == nullptr) {
        return
        {
            DaggyErrors::DataProviderTypeIsNotSupported,
            QString("Data provider type %1 is not supported")
                    .arg(data_source.type).toStdString()
        };
    }
    auto create_provider = fabric->create(data_source, this);
    if (!create_provider) {
        return create_provider.result();
    }

    connect(create_provider.value(), &IDataProvider::stateChanged, this, &DaggyCore::onDataProviderStateChanged);
    connect(create_provider.value(), &IDataProvider::error, this, &DaggyCore::onDataProviderError);

    connect(create_provider.value(), &IDataProvider::commandStateChanged, this, &DaggyCore::onCommandStateChanged);
    connect(create_provider.value(), &IDataProvider::commandError, this, &DaggyCore::onCommandError);
    connect(create_provider.value(), &IDataProvider::commandStream, this, &DaggyCore::onCommandStream);

    create_provider.value()->setObjectName(data_source.id);

    return create_provider.result();
}

void DaggyCore::setState(DaggyCore::State state)
{
    if (state_ == state)
        return;

    state_ = state;
    emit stateChanged(state_);
}


Result DaggyCore::addDataProvidersFabric(std::unique_ptr<IDataProviderFabric> new_fabric)
{
    if (data_provider_fabrics_.find(new_fabric->type) != data_provider_fabrics_.end())
        return Result{DaggyErrors::ProviderTypeAlreadyExists};
    const auto type = new_fabric->type;
    data_provider_fabrics_[type] = std::move(new_fabric);
    return Result::success;
}

Result DaggyCore::addDataAggregator(IDataAggregator* aggregator)
{
    aggregator->setParent(this);
    connect(this, &DaggyCore::dataProviderStateChanged, aggregator, &IDataAggregator::onDataProviderStateChanged);
    connect(this, &DaggyCore::dataProviderError, aggregator, &IDataAggregator::onDataProviderError);

    connect(this, &DaggyCore::commandStateChanged, aggregator, &IDataAggregator::onCommandStateChanged);
    connect(this, &DaggyCore::commandError, aggregator, &IDataAggregator::onCommandError);
    connect(this, &DaggyCore::commandStream, aggregator, &IDataAggregator::onCommandStream);

    return Result::success;
}

Result DaggyCore::addDataSourceConvertor(std::unique_ptr<daggyconv::IDataSourceConvertor> convertor)
{
    if (data_convertors_.find(convertor->type) != data_convertors_.end())
        return Result{DaggyErrors::ConvertorTypeAlreadyExists};
    const auto type = convertor->type;
    data_convertors_[type] = std::move(convertor);
    return Result::success;
}
