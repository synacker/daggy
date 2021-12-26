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

#include <memory>
#include <unordered_map>
#include <span>
#include <system_error>

#include <QMap>
#include <QObject>
#include <QString>

#include "Sources.hpp"


namespace daggy {

namespace providers {
class IFabric;
class IProvider;
}

namespace aggregators {
class IAggregator;
}

class DAGGYCORE_EXPORT Core : public QObject
{
    Q_OBJECT
    Q_PROPERTY(DaggyStates state READ state NOTIFY stateChanged)
    Q_PROPERTY(Sources sources READ sources CONSTANT)
    Q_PROPERTY(DaggyVersion version READ version CONSTANT)
    Q_PROPERTY(QString session READ session CONSTANT)
public:
    Core(Sources sources,
         QObject* parent = nullptr);
    Core(QString session,
         Sources sources,
         QObject* parent = nullptr);
    ~Core();

    DaggyVersion version() const noexcept;

    const Sources& sources() const noexcept;
    const QString& session() const noexcept;

    std::error_code start() noexcept;
    std::error_code stop() noexcept;

    DaggyStates state() const noexcept;

    std::error_code prepare();
    std::error_code prepare(std::span<providers::IFabric*> fabrics);

    std::error_code prepare(QString& error) noexcept;
    std::error_code prepare(std::span<providers::IFabric*> fabrics, QString& error) noexcept;

    std::error_code connectAggregator(aggregators::IAggregator* aggregator) noexcept;

signals:
    void stateChanged(DaggyStates state);

    void dataProviderStateChanged(QString provider_id,
                                  DaggyProviderStates state);
    void dataProviderError(QString provider_id,
                           std::error_code error_code);

    void commandStateChanged(QString provider_id,
                             QString command_id,
                             DaggyCommandStates state,
                             int exit_code);
    void commandStream(QString provider_id,
                       QString command_id,
                       sources::commands::Stream stream);
    void commandError(QString provider_id,
                      QString command_id,
                      std::error_code error_code);

private slots:
    void onDataProviderStateChanged(DaggyProviderStates state);
    void onDataProviderError(std::error_code error_code);

    void onCommandStateChanged(QString command_id,
                               DaggyCommandStates state,
                               int exit_code);
    void onCommandStream(QString command_id,
                         sources::commands::Stream stream);
    void onCommandError(QString command_id,
                        std::error_code error_code);

private:
    providers::IFabric* getFabric(const QString& type) const;

    QList<providers::IProvider*> getProviders() const;
    providers::IProvider* getProvider(const QString& provider_id) const;

    void setState(DaggyStates state);

    int activeDataProvidersCount() const;
    bool isActiveProvider(const providers::IProvider* const provider) const;

    void deleteAllProviders();

    const QString session_;
    Sources sources_;
    DaggyStates state_;
};

}
