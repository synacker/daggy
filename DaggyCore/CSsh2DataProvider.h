/*
Copyright 2017-2020 Milovidov Mikhail

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#pragma once
#include "IDataProvider.h"
#include "IDataProviderFabric.h"

#include <QHostAddress>

namespace daggyssh2 {
class Ssh2Client;
class Ssh2Process;
struct Ssh2Settings;
}

namespace daggycore {

class DAGGYCORE_EXPORT CSsh2DataProvider : public IDataProvider
{
    Q_OBJECT
public:
    CSsh2DataProvider(QHostAddress host,
                      const daggyssh2::Ssh2Settings& ssh2_settings,
                      Commands commands,
                      QObject *parent = nullptr);

    CSsh2DataProvider(QHostAddress host,
                      Commands commands,
                      QObject *parent = nullptr);

    ~CSsh2DataProvider();

    void start() override;
    void stop() override;

    constexpr static const char* provider_type = "ssh2";

    QString type() const override;

private:
    std::tuple<daggyssh2::Ssh2Process*, Command> getCommandContextFromSender() const;

    daggyssh2::Ssh2Process* ssh2Process(const QString& id) const;
    QPointer<daggyssh2::Ssh2Process> createProcess(const Command& getCommand);

    void onSsh2SessionStateChanged(const int state);
    void onSsh2ChannelsCountChanged(const int channels_count);

    void onSsh2ProcessStateChanged(const int process_state);
    void onSsh2ProcessNewDataChannel(QByteArray data, const int stream_id);
    void onSsh2ProcessError(const std::error_code& error_code);

    void startCommands();

    const QHostAddress host_;
    const quint16 port_;
    daggyssh2::Ssh2Client* ssh2_client_;
};

}
