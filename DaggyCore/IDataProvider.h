/*
Copyright 2017-2020 Milovidov Mikhail

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#pragma once

#include <QObject>
#include <map>

#include "daggycore_export.h"
#include "Command.h"

namespace daggycore {

class DAGGYCORE_EXPORT IDataProvider : public QObject
{
    Q_OBJECT
    Q_PROPERTY(State state READ state NOTIFY stateChanged)
public:
    enum State {
        NotStarted,
        Starting,
        FailedToStart,
        Started,
        Finishing,
        Finished
    };
    Q_ENUM(State)

    explicit IDataProvider(Commands commands,
                           QObject *parent = nullptr);
    virtual ~IDataProvider() = default;

    virtual void start() = 0;
    virtual void stop() = 0;

    virtual QString type() const = 0;

    const Commands& commands() const;
    Command getCommand(const QString& id) const;
    State state() const;

signals:
    void stateChanged(State state);

    void commandStateChanged(QString id,
                             Command::State state,
                             int exit_code);
    void commandStream(QString id,
                       Command::Stream);
    void commandError(QString id,
                      std::error_code error_code);

    void error(std::error_code error_code);

protected:
    void setState(State state);

private:
    const Commands commands_;
    State state_;
};
}
