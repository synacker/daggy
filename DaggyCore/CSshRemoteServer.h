/*
Copyright 2017-2018 Mikhail Milovidov <milovidovmikhail@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef CSSHREMOTESERVER_H
#define CSSHREMOTESERVER_H

#include <QObject>
#include <QSharedPointer>
#include <QVariantMap>

#include "IRemoteServer.h"

namespace QSsh {
    class SshConnectionParameters;
    class SshConnection;
    class SshRemoteProcess;
};

namespace daggycore {

class CSshRemoteServer : public IRemoteServer
{
    Q_OBJECT
public:
    CSshRemoteServer(const DataSource& data_source,
                     QObject* parent_pointer = nullptr);
    ~CSshRemoteServer() override;

    bool isForceKill() const;

    static constexpr const char* connection_type_global = "ssh";

private slots:
    void onHostConnected();
    void onHostDisconnected();
    void onHostError();

    void onNewStandardStreamData();
    void onNewErrorStreamData();

    void onCommandStarted();
    void onCommandWasExit(int exitStatus);

private:
    // IRemoteServer interface
    void startAgregator() override final;
    void stopAgregator(const bool hard_stop) override final;
    void reconnect() override final;

    void restartCommand(const QString& command_name) override final;

    void killConnection();
    void closeConnection();

    void startRemoteSshProcess(const QString& command_name, const QString& command);
    QSharedPointer<QSsh::SshRemoteProcess> getSshRemoteProcess(const QString& command_name) const;

    QSsh::SshConnection* const ssh_connection_pointer_;
    const int force_kill_;

    QMap<QString, QSharedPointer<QSsh::SshRemoteProcess>> ssh_processes_;
    QSharedPointer<QSsh::SshRemoteProcess> kill_childs_process_pointer_ = nullptr;
};

}
#endif // CSSHREMOTESERVER_H
