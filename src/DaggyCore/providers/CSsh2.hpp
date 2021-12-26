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

#include "IProvider.hpp"

namespace qtssh2 {
class Ssh2Client;
class Ssh2Process;
struct Ssh2Settings;
}

namespace daggy {
namespace providers {

class CSsh2 : public IProvider
{
    Q_OBJECT
public:
    CSsh2(const QString& session,
          QHostAddress host,
          const qtssh2::Ssh2Settings& ssh2_settings,
          sources::Commands commands,
          QObject *parent = nullptr);
    CSsh2(const QString& session,
          QHostAddress host,
          sources::Commands commands,
          QObject *parent = nullptr);
    ~CSsh2();

    std::error_code start() noexcept override;
    std::error_code stop() noexcept override;
    const QString& type() const noexcept override;

    static const QString provider_type;

private:
    void disconnectAll();

    qtssh2::Ssh2Process* ssh2Process(const QString& id) const;
    QPointer<qtssh2::Ssh2Process> createProcess(const sources::Command& command);

    void onSsh2SessionStateChanged(const int state);
    void onSsh2ChannelsCountChanged(const int channels_count);

    void onSsh2ProcessStateChanged(const int process_state);
    void onSsh2ProcessNewDataChannel(QByteArray data, const int stream_id);
    void onSsh2ProcessError(const std::error_code& error_code);

    void startCommands();

    const QHostAddress host_;
    const quint16 port_;
    qtssh2::Ssh2Client* ssh2_client_;
};

}
}



