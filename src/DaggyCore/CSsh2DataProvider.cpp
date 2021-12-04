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
#include "Precompiled.h"
#include "CSsh2DataProvider.h"

#include "Ssh2Client.h"
#include "Ssh2Process.h"

using namespace daggy;
using namespace daggyssh2;

namespace  {
constexpr const char* kill_command_global =
          "pids=$(pstree -p $PPID | grep -oP \"\\d+\" | grep -v $PPID | grep -v $$ | tac);"
          "for pid in $pids; do "
          "while kill -0 $pid; do "
          "kill -9 $pid;"
          "sleep 0.1;"
          "done "
          "done ";

const char* kill_command_id = "15397cd1-e80e-4584-9611-5398705fbd8e";

}

CSsh2DataProvider::CSsh2DataProvider(QHostAddress host,
                                     const Ssh2Settings& ssh2_settings,
                                     Commands commands,
                                     QObject* parent)
    : IDataProvider(std::move(commands), parent)
    , host_(std::move(host))
    , port_(ssh2_settings.port)
    , ssh2_client_(new Ssh2Client(ssh2_settings, this))
{
    connect(ssh2_client_, &Ssh2Client::sessionStateChanged, this, &CSsh2DataProvider::onSsh2SessionStateChanged);
    connect(ssh2_client_, &Ssh2Client::channelsCountChanged, this, &CSsh2DataProvider::onSsh2ChannelsCountChanged);
    connect(ssh2_client_, &Ssh2Client::ssh2Error, this, &CSsh2DataProvider::error);
}

CSsh2DataProvider::CSsh2DataProvider(QHostAddress host,
                                     Commands commands,
                                     QObject* parent)
    : CSsh2DataProvider(std::move(host),
                        Ssh2Settings(),
                        std::move(commands),
                        parent)
{

}

CSsh2DataProvider::~CSsh2DataProvider()
{
}

void CSsh2DataProvider::start()
{
    ssh2_client_->connectToHost(host_, port_);
}

void CSsh2DataProvider::stop()
{
    disconnectAll();
}

QString CSsh2DataProvider::type() const
{
    return provider_type;
}

void CSsh2DataProvider::disconnectAll()
{
    if (ssh2Process(kill_command_id) != nullptr || state() != IDataProvider::Started)
        return;

    auto terminate_process = ssh2_client_->createProcess(kill_command_global);
    terminate_process->setObjectName(kill_command_id);
    connect(terminate_process, &Ssh2Process::processStateChanged,
         [this](const Ssh2Process::ProcessStates state)
    {
      switch (state) {
      case Ssh2Process::ProcessStates::Finished:
      case Ssh2Process::ProcessStates::FailedToStart:
           ssh2_client_->disconnectFromHost();
           break;
      default:;
      }
    });
    terminate_process->open();
}

std::tuple<Ssh2Process*, Command> CSsh2DataProvider::getCommandContextFromSender() const
{
    Ssh2Process* ssh2_process = qobject_cast<Ssh2Process*>(sender());
    const QString& id = ssh2_process->objectName();
    Command command = getCommand(id);
    return std::tie(ssh2_process, command);
}

daggyssh2::Ssh2Process* CSsh2DataProvider::ssh2Process(const QString& id) const
{
    return findChild<Ssh2Process*>(id);
}

QPointer<Ssh2Process> CSsh2DataProvider::createProcess(const Command& command)
{
    QPointer<Ssh2Process> ssh2_process = ssh2_client_->createProcess(command.exec);
    ssh2_process->setObjectName(command.id);
    connect(ssh2_process, &Ssh2Process::processStateChanged, this, &CSsh2DataProvider::onSsh2ProcessStateChanged);
    connect(ssh2_process, &Ssh2Process::newChannelData, this, &CSsh2DataProvider::onSsh2ProcessNewDataChannel);
    connect(ssh2_process, &Ssh2Process::ssh2Error, this, &CSsh2DataProvider::onSsh2ProcessError);
    return ssh2_process;
}

void CSsh2DataProvider::onSsh2SessionStateChanged(const int state)
{
    switch (static_cast<Ssh2Client::SessionStates>(state)) {
    case Ssh2Client::StartingSession:
    case Ssh2Client::GetAuthMethods:
    case Ssh2Client::Authentication:
        setState(Starting);
        break;
    case Ssh2Client::Established:
        setState(Started);
        startCommands();
        break;
    case Ssh2Client::FailedToEstablshed:
        setState(FailedToStart);
        break;
    case Ssh2Client::Closing:
        setState(Finishing);
        break;
    case Ssh2Client::Closed:
    case Ssh2Client::Aborted:
        setState(Finished);
        break;
    case Ssh2Client::NotEstableshed:
        setState(NotStarted);
        break;
    }
}

void CSsh2DataProvider::onSsh2ChannelsCountChanged(const int channels_count)
{
    if (channels_count == 0)
        ssh2_client_->disconnectFromHost();
}

void CSsh2DataProvider::onSsh2ProcessStateChanged(const int process_state)
{
    auto [ssh2_process, command] = getCommandContextFromSender();
    Command::State command_state = Command::NotStarted;
    int exit_code = 0;
    switch (static_cast<Ssh2Process::ProcessStates>(process_state)) {
    case Ssh2Process::NotStarted:
        command_state = Command::NotStarted;
        break;
    case Ssh2Process::Starting:
        command_state = Command::Starting;
        break;
    case Ssh2Process::Started:
        command_state = Command::Started;
        break;
    case Ssh2Process::FailedToStart:
        command_state = Command::FailedToStart;
        ssh2_process->deleteLater();
        break;
    case Ssh2Process::Finishing:
        command_state = Command::Finishing;
        break;
    case Ssh2Process::Finished:
        command_state = Command::Finished;
        exit_code = ssh2_process->exitStatus();
        break;
    }
    emit commandStateChanged(command.id, command_state, exit_code);

    if (process_state == Ssh2Process::Finished) {
        if (command.restart && state() == IDataProvider::Started)
            ssh2_process->open();
        else
            ssh2_process->deleteLater();
    }
}

void CSsh2DataProvider::onSsh2ProcessNewDataChannel(QByteArray data, const int stream_id)
{
    auto [ssh2_process, command] = getCommandContextFromSender();
    Command::Stream::Type command_stream_type = Command::Stream::Type::Standard;
    switch (static_cast<Ssh2Channel::ChannelStream>(stream_id)) {
    case Ssh2Channel::Out:
        command_stream_type = Command::Stream::Type::Standard;
        break;
    case Ssh2Channel::Err:
        command_stream_type = Command::Stream::Type::Error;
        break;

    }
    emit commandStream(command.id,
                      {
                           command.extension,
                           data,
                           command_stream_type
                       });
}

void CSsh2DataProvider::onSsh2ProcessError(const std::error_code& error_code)
{
    emit commandError(sender()->objectName(), error_code);
}

void CSsh2DataProvider::startCommands()
{
    for (const auto& command : commands()) {
        Ssh2Process* ssh2_process = ssh2Process(command.id);
        if (ssh2_process == nullptr)
            ssh2_process = createProcess(command);
        ssh2_process->open();
    }
}
