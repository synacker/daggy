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

#include <QIODevice>

#include "Ssh2Types.h"

namespace daggyssh2 {
class Ssh2Client;

class Ssh2Channel : public QIODevice
{
    Q_OBJECT
    Q_ENUMS(ChannelStates)
    Q_PROPERTY(ChannelStates channelState READ channelState NOTIFY channelStateChanged)
public:
    enum ChannelStates {
        NotOpen,
        Opening,
        FailedToOpen,
        Opened,
        Closing,
        Closed
    };

    enum ChannelStream {
        Out,
        Err
    };

    ~Ssh2Channel();

    qint64 writeData(const char *data, qint64 len) override;
    qint64 readData(char *data, qint64 maxlen) override;

    bool isSequential() const override;

    bool open(OpenMode mode = QIODevice::ReadWrite) override;
    void close() override;

    Ssh2Client* ssh2Client() const;
    LIBSSH2_CHANNEL* ssh2Channel() const;

    virtual void checkIncomingData();

    int exitStatus() const;

    ChannelStates channelState() const;

    QString exitSignal() const;

signals:
    void ssh2Error(std::error_code ssh2_error);

    void channelStateChanged(ChannelStates channelState);

    void newChannelData(QByteArray data, ChannelStream stream_id);

protected:
    Ssh2Channel(Ssh2Client* ssh2_client);

    std::error_code setLastError(const std::error_code& error_code);

private:
    void setSsh2ChannelState(const ChannelStates& state);
    std::error_code openSession();
    std::error_code closeSession();
    void destroyChannel();
    void checkChannelData(const ChannelStream& stream_id);

    ChannelStates ssh2_channel_state_;
    LIBSSH2_CHANNEL* ssh2_channel_;
    int exit_status_;
    QString exit_signal_;
    std::error_code last_error_;
    void checkChannelData();
};

}
