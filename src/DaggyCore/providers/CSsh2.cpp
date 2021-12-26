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
#include "../Precompiled.hpp"
#include "CSsh2.hpp"
#include "ssh2/Ssh2Client.hpp"
#include "ssh2/Ssh2Process.hpp"
#include "../Errors.hpp"

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

const QString daggy::providers::CSsh2::provider_type("ssh2");

using namespace qtssh2;

daggy::providers::CSsh2::CSsh2(const QString& session,
                               QHostAddress host,
                               const Ssh2Settings& ssh2_settings,
                               sources::Commands commands,
                               QObject* parent)
    : IProvider(session, std::move(commands), parent)
    , host_(std::move(host))
    , port_(ssh2_settings.port)
    , ssh2_client_(new Ssh2Client(ssh2_settings, this))
{
    connect(ssh2_client_, &Ssh2Client::sessionStateChanged, this, &CSsh2::onSsh2SessionStateChanged);
    connect(ssh2_client_, &Ssh2Client::channelsCountChanged, this, &CSsh2::onSsh2ChannelsCountChanged);
    connect(ssh2_client_, &Ssh2Client::ssh2Error, this, &CSsh2::error);
}

daggy::providers::CSsh2::CSsh2(const QString& session,
                               QHostAddress host,
                               sources::Commands commands,
                               QObject* parent)
    : CSsh2(session, std::move(host), Ssh2Settings(), std::move(commands), parent)
{

}

daggy::providers::CSsh2::~CSsh2()
{

}

std::error_code daggy::providers::CSsh2::start() noexcept
{
    std::error_code result = errors::success;
    switch (state()) {
    case DaggyProviderNotStarted:
    case DaggyProviderFinished:
    case DaggyProviderFailedToStart:
        ssh2_client_->connectToHost(host_, port_);
        break;
    case DaggyProviderStarting:
    case DaggyProviderStarted:
    case DaggyProviderFinishing:
        result = errors::make_error_code(DaggyErrorAlreadyStarted);
        break;

    }
    return result;
}

std::error_code daggy::providers::CSsh2::stop() noexcept
{
    std::error_code result;
    switch (state()) {
    case DaggyProviderNotStarted:
    case DaggyProviderFailedToStart:
    case DaggyProviderFinished:
        result = errors::make_error_code(DaggyErrorAlreadyFinished);
        break;
    case DaggyProviderStarting:
    case DaggyProviderStarted:
    case DaggyProviderFinishing:
        disconnectAll();
        break;

    }
    return result;
}

const QString& daggy::providers::CSsh2::type() const noexcept
{
    return provider_type;
}

void daggy::providers::CSsh2::disconnectAll()
{
    if (ssh2Process(kill_command_id) != nullptr || state() != DaggyProviderStarted)
        return;

    auto terminate_process = ssh2_client_->createProcess(kill_command_global);
    terminate_process->setObjectName(kill_command_id);
    connect(terminate_process, &Ssh2Process::processStateChanged, ssh2_client_,
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

Ssh2Process* daggy::providers::CSsh2::ssh2Process(const QString& id) const
{
    return findChild<Ssh2Process*>(id);
}

QPointer<Ssh2Process> daggy::providers::CSsh2::createProcess(const sources::Command& command)
{
    QPointer<Ssh2Process> ssh2_process = ssh2_client_->createProcess(command.second.exec);
    const auto& id = command.first;
    ssh2_process->setObjectName(id);
    connect(ssh2_process, &Ssh2Process::processStateChanged, this, &CSsh2::onSsh2ProcessStateChanged);
    connect(ssh2_process, &Ssh2Process::newChannelData, this, &CSsh2::onSsh2ProcessNewDataChannel);
    connect(ssh2_process, &Ssh2Process::ssh2Error, this, &CSsh2::onSsh2ProcessError);
    return ssh2_process;
}

void daggy::providers::CSsh2::onSsh2SessionStateChanged(const int state)
{
    switch (static_cast<Ssh2Client::SessionStates>(state)) {
    case Ssh2Client::StartingSession:
    case Ssh2Client::GetAuthMethods:
    case Ssh2Client::Authentication:
        setState(DaggyProviderStarting);
        break;
    case Ssh2Client::Established:
        setState(DaggyProviderStarted);
        startCommands();
        break;
    case Ssh2Client::FailedToEstablshed:
        setState(DaggyProviderFailedToStart);
        break;
    case Ssh2Client::Closing:
        setState(DaggyProviderFinishing);
        break;
    case Ssh2Client::Closed:
    case Ssh2Client::Aborted:
        setState(DaggyProviderFinished);
        break;
    case Ssh2Client::NotEstableshed:
        setState(DaggyProviderNotStarted);
        break;
    }
}

void daggy::providers::CSsh2::onSsh2ChannelsCountChanged(const int channels_count)
{
    if (channels_count == 0)
        ssh2_client_->disconnectFromHost();
}

void daggy::providers::CSsh2::onSsh2ProcessStateChanged(const int process_state)
{
    Ssh2Process* ssh2_process = qobject_cast<Ssh2Process*>(sender());
    const auto& command_id = ssh2_process->objectName();
    const auto& command_properties = getCommandProperties(command_id);

    DaggyCommandStates command_state = DaggyCommandNotStarted;
    int exit_code = 0;
    switch (static_cast<Ssh2Process::ProcessStates>(process_state)) {
    case Ssh2Process::NotStarted:
        command_state = DaggyCommandNotStarted;
        break;
    case Ssh2Process::Starting:
        command_state = DaggyCommandStarting;
        break;
    case Ssh2Process::Started:
    {
        metaStream(command_id, DaggyStreamStandard, true);
        metaStream(command_id, DaggyStreamError, true);
        command_state = DaggyCommandStarted;
    }
        break;
    case Ssh2Process::FailedToStart:
        command_state = DaggyCommandFailedToStart;
        ssh2_process->deleteLater();
        break;
    case Ssh2Process::Finishing:
        command_state = DaggyCommandFinishing;
        break;
    case Ssh2Process::Finished:
        command_state = DaggyCommandFinished;
        exit_code = ssh2_process->exitStatus();
        break;
    }
    emit commandStateChanged(command_id, command_state, exit_code);

    if (process_state == Ssh2Process::Finished) {
        if (command_properties.restart && state() == DaggyProviderStarted) {
            ssh2_process->open();
        } else
            ssh2_process->deleteLater();
    }
}

void daggy::providers::CSsh2::onSsh2ProcessNewDataChannel(QByteArray data, const int stream_id)
{
    Ssh2Process* ssh2_process = qobject_cast<Ssh2Process*>(sender());
    const auto& command_id = ssh2_process->objectName();
    const auto& command_properties = getCommandProperties(command_id);
    DaggyStreamTypes command_stream_type = DaggyStreamStandard;
    switch (static_cast<Ssh2Channel::ChannelStream>(stream_id)) {
    case Ssh2Channel::Out:
        command_stream_type = DaggyStreamTypes::DaggyStreamStandard;
        break;
    case Ssh2Channel::Err:
        command_stream_type = DaggyStreamTypes::DaggyStreamError;
        break;

    }
    emit commandStream(command_id,
    {
       metaStream(command_id, command_stream_type),
       std::move(data)
    });
}

void daggy::providers::CSsh2::onSsh2ProcessError(const std::error_code& error_code)
{
    emit commandError(sender()->objectName(), error_code);
}

void daggy::providers::CSsh2::startCommands()
{
    const auto& start_commands = commands();
    auto it = start_commands.begin();
    while (it != start_commands.end()) {
        Ssh2Process* ssh2_process = ssh2Process(it.key());
        if (!ssh2_process)
            ssh2_process = createProcess({it.key(), it.value()});
        ssh2_process->open();
        it++;
    }
}
