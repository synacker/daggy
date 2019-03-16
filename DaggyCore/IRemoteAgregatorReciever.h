/*
Copyright 2017-2018 Mikhail Milovidov <milovidovmikhail@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef IREMOTEAGREGATORRECIEVER_H
#define IREMOTEAGREGATORRECIEVER_H

#include <QObject>

#include "daggycore_global.h"
#include "RemoteCommand.h"
#include "RemoteConnectionStatus.h"

namespace daggycore {

class DAGGYCORESHARED_EXPORT IRemoteAgregatorReciever : public QObject
{
    Q_OBJECT
public:
    IRemoteAgregatorReciever(QObject* const parent_ptr = nullptr)
        : QObject(parent_ptr) {}
    virtual ~IRemoteAgregatorReciever() = default;

public slots:
    virtual void onConnectionStatusChanged(const QString server_name,
                                           const RemoteConnectionStatus status,
                                           const QString message) = 0;
    virtual void onRemoteCommandStatusChanged(const QString server_name,
                                              const RemoteCommand remote_command,
                                              const RemoteCommand::Status status,
                                              const int exit_code) = 0;
    virtual void onNewRemoteCommandStream(const QString server_name,
                                          const RemoteCommand::Stream stream) = 0;


};

}

#endif // IREMOTEAGREGATORRECIEVER_H
