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

#include "Ssh2Channel.hpp"

namespace qtssh2 {

class Ssh2Process : public Ssh2Channel
{
    Q_OBJECT
    Q_ENUMS(ProcessStates)
    Q_PROPERTY(ProcessStates processState READ processState NOTIFY processStateChanged)
public:
    enum ProcessStates {
        NotStarted,
        Starting,
        Started,
        FailedToStart,
        Finishing,
        Finished
    };

    Ssh2Process() = delete;
    Ssh2Process(const QString& command,
                Ssh2Client* ssh2_client);

    ProcessStates processState() const;

signals:
    void processStateChanged(ProcessStates processState);

protected:

    void checkIncomingData() override;

private slots:
    void onSsh2ChannelStateChanged(ChannelStates state);

private:
    bool setSsh2ProcessState(ProcessStates ssh2_process_state);
    std::error_code execSsh2Process();

    const QString command_;
    ProcessStates ssh2_process_state_;
};

}
