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
#include "Precompiled.hpp"
#include "Core.hpp"
#include "Core.h"
#include <DaggyCore/version.h>

#include "providers/IProvider.hpp"
#include "providers/CLocalFabric.hpp"
#include "providers/CLocal.hpp"

#include "providers/CSshFabric.hpp"
#include "providers/CSsh.hpp"

#ifdef SSH2_SUPPORT
#include "providers/CSsh2Fabric.hpp"
#include "providers/CSsh2.hpp"
#endif

#include "aggregators/IAggregator.hpp"

using namespace daggy;

Core::Core(Sources sources,
           QObject* parent)
    : Core(QUuid::createUuid().toString(), std::move(sources), parent)
{
}

Core::Core(QString session,
           Sources sources,
           QObject* parent)
    : QObject(parent)
    , session_(std::move(session))
    , sources_(std::move(sources))
    , state_(DaggyNotStarted)
{

}

Core::~Core()
{
    deleteAllProviders();
}

DaggyVersion Core::version() const noexcept
{
    return
    {
        DAGGY_VERSION_FULL,
        DAGGY_VERSION_MAJOR,
        DAGGY_VERSION_MINOR,
        DAGGY_VERSION_PATCH,
        DAGGY_VERSION_TWEAK,
        DAGGY_VERSION_BRANCH,
        DAGGY_VENDOR,
        DAGGY_VERSION_COMMIT
    };
}

const Sources& Core::sources() const noexcept
{
    return sources_;
}

const QString& Core::session() const noexcept
{
    return session_;
}


int Core::activeDataProvidersCount() const
{
    int result = 0;
    for (providers::IProvider* provider : getProviders())
        if (isActiveProvider(provider))
            result++;
    return result;
}

bool Core::isActiveProvider(const providers::IProvider* const provider) const
{
    bool result = false;
    switch (provider->state()) {
    case DaggyProviderNotStarted:
    case DaggyProviderFailedToStart:
    case DaggyProviderFinished:
        result = false;
        break;
    default:
        result = true;
    }
    return result;
}

void Core::deleteAllProviders()
{
    const auto& providers = getProviders();
    for (auto provider : providers)
        delete provider;
}

std::error_code Core::start() noexcept
try
{
    if (state() == DaggyStarted || state() == DaggyFinishing)
        return errors::make_error_code(DaggyErrorAlreadyStarted);

    auto providers = getProviders();
    if (providers.empty()) {
        setState(DaggyFinished);
        return errors::make_error_code(DaggyErrorSourceNullCommand);
    }

    setState(DaggyStarted);
    foreach (providers::IProvider* provider, providers) {
        provider->start();
    }

    return errors::success;
}
catch (const std::system_error& exception)
{
    return exception.code();
}
catch (...) {
    return errors::make_error_code(DaggyErrorInternal);
}

std::error_code Core::stop() noexcept
try {
    if (state() == DaggyNotStarted || state() == DaggyFinished)
        return errors::make_error_code(DaggyErrorAlreadyFinished);

    const auto& data_providers = getProviders();
    if (data_providers.empty() || activeDataProvidersCount() == 0) {
        setState(DaggyFinished);
        return errors::success;
    }

    setState(DaggyFinishing);
    for (providers::IProvider* provider : data_providers)
        provider->stop();
    return errors::success;
}
catch (const std::system_error& exception)
{
    return exception.code();
}
catch (...)
{
    return errors::make_error_code(DaggyErrorInternal);
}

DaggyStates Core::state() const noexcept
{
    return state_;
}

std::error_code Core::prepare()
{
    QString message;
    auto error = prepare(message);
    if (error)
        throw std::runtime_error(message.toStdString());
    return error;
}

std::error_code Core::prepare(QString& error) noexcept
{
    return prepare(nullptr, 0, error);
}

std::error_code Core::prepare(std::vector<providers::IFabric*> fabrics)
{
    QString message;
    auto error = prepare(std::move(fabrics), message);
    if (error)
        throw std::runtime_error(message.toStdString());
    return error;
}

std::error_code Core::prepare(std::vector<providers::IFabric*> fabrics, QString& error) noexcept
{
    return prepare(fabrics.data(), fabrics.size(), error);
}

#if __cplusplus > 201703L
std::error_code Core::prepare(std::span<providers::IFabric*> fabrics)
{
    QString message;
    auto error = prepare(std::move(fabrics), message);
    if (error)
        throw std::runtime_error(message.toStdString());
    return error;
}

std::error_code Core::prepare(std::span<providers::IFabric*> fabrics, QString& error) noexcept
{
    return prepare(fabrics.data(), fabrics.size(), error);
}
#endif

void Core::onDataProviderStateChanged(DaggyProviderStates state)
{
    const QString& provider_id = sender()->objectName();
    emit dataProviderStateChanged(provider_id,
                                  state);

    const auto& source = sources_[provider_id];
    if (state == DaggyProviderFinished &&
        source.reconnect &&
        state_ == DaggyStarted)
    {
        auto* provider = getProvider(provider_id);
        provider->start();
    }

    if (activeDataProvidersCount() == 0) {
        setState(DaggyFinished);
    }
}

void Core::onDataProviderError(std::error_code error_code)
{
    emit dataProviderError(sender()->objectName(),
                           error_code);
}

void Core::onCommandStateChanged(QString command_id,
                                      DaggyCommandStates state,
                                      int exit_code)
{
    emit commandStateChanged(sender()->objectName(),
                             command_id,
                             state,
                             exit_code);
}

void Core::onCommandStream(QString command_id,
                                sources::commands::Stream stream)
{
    emit commandStream(sender()->objectName(),
                       command_id,
                       stream);
}

void Core::onCommandError(QString command_id,
                          std::error_code error_code)
{
    emit commandError(sender()->objectName(),
                      command_id,
                      error_code);
}

std::error_code Core::prepare(providers::IFabric** fabrics, const size_t size, QString& error) noexcept
try {
    const auto& providers = getProviders();
    if (!providers.isEmpty())
        return errors::success;

    std::unordered_map<QString, providers::IFabric*> fabrics_map;
    static thread_local providers::CLocalFabric local_fabric;
    fabrics_map[providers::CLocal::provider_type] = &local_fabric;

    static thread_local providers::CSshFabric ssh_fabric;
    fabrics_map[providers::CSsh::provider_type] = &ssh_fabric;

#ifdef SSH2_SUPPORT
    static thread_local providers::CSsh2Fabric ssh2_fabric;
    fabrics_map[providers::CSsh2::provider_type] = &ssh2_fabric;
#endif
    if (fabrics) {
        for (size_t index = 0; index < size; index++)
            fabrics_map[fabrics[index]->type()] = fabrics[index];
    }

    auto source = sources_.cbegin();
    while(source != sources_.cend()) {
        const auto& source_id = source.key();
        const auto& properties = source.value();

        const auto& fabric = fabrics_map.find(properties.type);

        if (fabric == fabrics_map.cend()) {
            throw std::system_error(errors::make_error_code(DaggyErrorSourceProviderTypeIsNotSupported),
                                    QString("Data provider type %1 is not supported").arg(properties.type).toStdString());
        }

        auto provider = fabric->second->create(session_, {source_id, properties}, this);
        if (!provider) {
            throw std::system_error(provider.error,
                                    provider.message.toStdString());
        }

        connect(*provider, &providers::IProvider::stateChanged, this, &Core::onDataProviderStateChanged);
        connect(*provider, &providers::IProvider::error, this, &Core::onDataProviderError);

        connect(*provider, &providers::IProvider::commandStateChanged, this, &Core::onCommandStateChanged);
        connect(*provider, &providers::IProvider::commandError, this, &Core::onCommandError);
        connect(*provider, &providers::IProvider::commandStream, this, &Core::onCommandStream);

        (*provider)->setObjectName(source_id);

        source++;
    }
    return errors::success;
}
catch (const std::system_error& exception)
{
    deleteAllProviders();
    error = QString::fromStdString(exception.what());
    return exception.code();
} catch (const std::exception& exception)
{
    error = QString::fromStdString(exception.what());
    return errors::make_error_code(DaggyErrorProviderCannotPrepare);
} catch (...)
{
    error = QString::fromStdString("unknown error");
    return errors::make_error_code(DaggyErrorProviderCannotPrepare);
}

QList<providers::IProvider*> Core::getProviders() const
{
    return findChildren<providers::IProvider*>();
}

providers::IProvider* Core::getProvider(const QString& provider_id) const
{
    providers::IProvider* result = nullptr;
    if (!provider_id.isEmpty())
        result = findChild<providers::IProvider*>(provider_id);
    return result;
}

void Core::setState(DaggyStates state)
{
    if (state_ == state)
        return;

    state_ = state;
    emit stateChanged(state_);
}

std::error_code Core::connectAggregator(aggregators::IAggregator* aggregator) noexcept
try {
    if (!aggregator->isReady())
        return errors::make_error_code(DaggyErrorAggregatorCannotConnect);

    if (connect(this, &Core::dataProviderStateChanged, aggregator, &aggregators::IAggregator::onDataProviderStateChanged) &&
        connect(this, &Core::dataProviderError, aggregator, &aggregators::IAggregator::onDataProviderError) &&
        connect(this, &Core::commandStateChanged, aggregator, &aggregators::IAggregator::onCommandStateChanged) &&
        connect(this, &Core::commandError, aggregator, &aggregators::IAggregator::onCommandError) &&
        connect(this, &Core::commandStream, aggregator, &aggregators::IAggregator::onCommandStream) &&
        connect(this, &Core::stateChanged, aggregator, &aggregators::IAggregator::onDaggyStateChanged)
    )
        return errors::success;

    return errors::make_error_code(DaggyErrorAggregatorCannotConnect);
}
catch (...)
{
    return errors::make_error_code(DaggyErrorAggregatorCannotConnect);
}
