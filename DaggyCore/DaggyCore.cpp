/*
Copyright 2017-2020 Milovidov Mikhail

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "Precompiled.h"
#include "DaggyCore.h"

#include "IDataProviderFabric.h"
#include "IDataAggregator.h"
#include "IDataProvider.h"
#include "IDataSourceConvertor.h"

using namespace daggycore;

DaggyCore::DaggyCore(DataSources data_sources,
                     QObject* parent)
    : QObject(parent)
    , data_sources_(std::move(data_sources))
    , state_(NotStarted)
{
}

DaggyCore::DaggyCore(QObject* parent)
    : QObject(parent)
{

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

    data_sources_ = convertion.value();
    return Result::success;
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
    auto data_providers = getProviders();
    if (data_providers.empty()) {
        for (const auto& data_source : data_sources_) {
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

    for (IDataProvider* provider : data_providers)
        provider->start();

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

void DaggyCore::onDataProviderStateChanged(const int state)
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

    if (activeDataProvidersCount() == 0)
        setState(Finished);
}

void DaggyCore::onDataProviderError(const std::error_code error_code)
{
    emit dataProviderError(sender()->objectName(),
                           error_code);
}

void DaggyCore::onCommandStateChanged(const QString command_id,
                                      const Command::State state,
                                      const int exit_code)
{
    emit commandStateChanged(sender()->objectName(),
                             command_id,
                             state,
                             exit_code);
}

void DaggyCore::onCommandStream(const QString command_id,
                                const Command::Stream stream)
{
    emit commandStream(sender()->objectName(),
                       command_id,
                       stream);
}

void DaggyCore::onCommandError(const QString command_id,
                               const std::error_code error_code)
{
    emit commandError(sender()->objectName(),
                      command_id,
                      error_code);
}

IDataProviderFabric* DaggyCore::getFabric(const QString& type) const
{
    IDataProviderFabric* result = nullptr;
    if (!type.isEmpty())
        result = findChild<IDataProviderFabric*>(type);
    return result;
}

daggyconv::IDataSourceConvertor* DaggyCore::getConvertor(const QString& type) const
{
    daggyconv::IDataSourceConvertor* result = nullptr;
    if (!type.isEmpty())
        result = findChild<daggyconv::IDataSourceConvertor*>(type);
    return result;
}

QList<IDataAggregator*> DaggyCore::getAggregators() const
{
    return findChildren<IDataAggregator*>();
}

QList<IDataProviderFabric*> DaggyCore::getFabrics() const
{
    return findChildren<IDataProviderFabric*>();
}

QList<IDataProvider*> DaggyCore::getProviders() const
{
    return findChildren<IDataProvider*>();
}

QList<daggyconv::IDataSourceConvertor*> DaggyCore::getConvertors() const
{
    return findChildren<daggyconv::IDataSourceConvertor*>();
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

    return create_provider.result();
}

void DaggyCore::setState(DaggyCore::State state)
{
    if (state_ == state)
        return;

    state_ = state;
    emit stateChanged(state_);
}


Result DaggyCore::addDataProvidersFabric(IDataProviderFabric* new_fabric)
{
    new_fabric->setParent(this);
    new_fabric->setObjectName(new_fabric->type);

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

Result DaggyCore::addDataSourceConvertor(daggyconv::IDataSourceConvertor* convertor)
{
    convertor->setParent(this);
    convertor->setObjectName(convertor->type);

    return Result::success;
}
