/*
Copyright 2017-2018 Mikhail Milovidov <milovidovmikhail@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef IREMOTEAGREGATOR_H
#define IREMOTEAGREGATOR_H

#include <QObject>
#include "daggycore_global.h"

#include "RemoteCommand.h"
#include "RemoteConnectionStatus.h"

namespace daggycore {

class DAGGYCORESHARED_EXPORT IRemoteAgregator : public QObject
{
    Q_OBJECT
public:
    enum class State {
        Stopped,
        Run,
        Stopping
    };

    explicit IRemoteAgregator(QObject *pParent = nullptr);
    virtual ~IRemoteAgregator() = default;

    bool isExistsRunningRemoteCommands() const;

    virtual size_t runingRemoteCommandsCount() const = 0;

    void start();
    void stop();

    State state() const;

signals:
    void connectionStatusChanged(QString server_name,
                                 RemoteConnectionStatus status,
                                 QString message);
    void remoteCommandStatusChanged(QString server_name,
                                    RemoteCommand remote_command,
                                    RemoteCommand::Status status,
                                    int exit_code);
    void newRemoteCommandStream(QString server_id, RemoteCommand::Stream stream);

    void stateChanged(State state);

protected:
    virtual void startAgregator() = 0;
    virtual void stopAgregator() = 0;

    void setStopped();

private:
    void setState(const State state);
    State state_;
    size_t running_remote_command_count_;
};

}

#endif // IREMOTEAGREGATOR_H
