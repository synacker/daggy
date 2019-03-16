/*
Copyright 2017-2018 Mikhail Milovidov <milovidovmikhail@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef ISERVERCONNECTION_H
#define ISERVERCONNECTION_H

#include "daggycore_global.h"

#include <QObject>
#include <QString>
#include <QVector>
#include <QMap>

#include "IRemoteAgregator.h"
#include "DataSource.h"

namespace daggycore {

class DAGGYCORESHARED_EXPORT IRemoteServer : public IRemoteAgregator
{
    Q_OBJECT
public:
    IRemoteServer(const DataSource& data_source,
                  QObject* parent_ptr = nullptr);
    ~IRemoteServer() override = default;

    const QString& serverName() const;
    const QString& connectionType() const;
    const QVariantMap& connectionParameters() const;

    const DataSource& dataSource() const;

    RemoteConnectionStatus connectionStatus() const;
    RemoteCommand::Status commandStatus(const QString& commandName) const;
    const RemoteCommand& getRemoteCommand(const QString& commandName) const;

    bool isExistsRestartCommand() const;

    size_t runingRemoteCommandsCount() const override final;

protected:
    virtual void restartCommand(const QString& commandName) = 0;
    virtual void reconnect() = 0;

    void setConnectionStatus(const RemoteConnectionStatus status, const QString& message = QString());
    void setRemoteCommandStatus(const QString& commandName, const RemoteCommand::Status commandStatus, const int exit_code = 0);
    void setNewRemoteCommandStream(const QString& commandName, const QByteArray& data, const RemoteCommand::Stream::Type type);

private:
    void startCommands();
    bool isExistsRestartCommand(const std::vector<RemoteCommand>& commands) const;
    std::map<QString, RemoteCommand> convertRemoteCommands(const std::vector<RemoteCommand>& remoteCommands) const;

    const DataSource data_source_;
    const std::map<QString, RemoteCommand> remote_commands_;
    const bool exists_restart_commands_;
    QMap<QString, RemoteCommand::Status> commands_status_;

    RemoteConnectionStatus connection_status_ = RemoteConnectionStatus::NotConnected;
};

}

#endif // ISERVERCONNECTION_H
