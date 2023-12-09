/*
MIT License

Copyright (c) 2021 Mikhail Milovidov

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

#include <QObject>

#include "../Sources.hpp"

namespace daggy {
namespace providers {

class DAGGYCORE_EXPORT IProvider : public QObject
{
    Q_OBJECT
    Q_PROPERTY(DaggyProviderStates state READ state NOTIFY stateChanged)
    Q_PROPERTY(sources::Commands commands READ commands CONSTANT)
public:
    IProvider(const QString& session,
              sources::Commands commands,
              QObject* parent = nullptr);

    virtual ~IProvider();

    virtual std::error_code start() noexcept = 0;
    virtual std::error_code stop() noexcept = 0;

    virtual const QString& type() const noexcept = 0;

    const sources::Commands& commands() const noexcept;
    bool commandSupported(const QString& id) const noexcept;
    const sources::commands::Properties& getCommandProperties(const QString& id) const noexcept;
    DaggyProviderStates state() const noexcept;

    int restartCommandsCount() const noexcept;

    const QString& session() const;

signals:
    void stateChanged(DaggyProviderStates state);

    void commandStateChanged(QString id,
                             DaggyCommandStates state,
                             int exit_code);
    void commandStream(QString id,
                       sources::commands::Stream);
    void commandError(QString id,
                      std::error_code error_code);

    void error(std::error_code error_code);

protected:
    void setState(DaggyProviderStates state);
    sources::commands::streams::Meta metaStream(const QString& id, DaggyStreamTypes type, bool reset = false);


private:
    const QString session_;
    const sources::Commands commands_;
    DaggyProviderStates state_;

    QMap<QString, sources::commands::streams::Meta> streams_meta_;
};

}
}

