#include "Precompiled.h"
#include "IRemoteAgregator.h"

IRemoteAgregator::IRemoteAgregator(QObject* pParent)
    : QObject(pParent)
    , state_(State::Stopped)
    , running_remote_command_count_(0)
{
}

bool IRemoteAgregator::isExistsRunningRemoteCommands() const
{
    return runingRemoteCommandsCount() > 0;
}

size_t IRemoteAgregator::runingRemoteCommandsCount() const
{
    return running_remote_command_count_;
}

void IRemoteAgregator::start()
{
    if (state_ == State::Stopped) {
        setState(State::Run);
        startAgregator();
    }
}

void IRemoteAgregator::stop()
{
    if (state_ == State::Run) {
        setState(State::Stopping);
        stopAgregator();
    }
}

void IRemoteAgregator::setState(const IRemoteAgregator::State state)
{
    if (state_ != state) {
        state_ = state;
        emit stateChanged(state);
    }
}

void IRemoteAgregator::onRemoteCommandStatusChanged(QString, QString, const RemoteCommand::Status status, int)
{
    if (status == RemoteCommand::Status::Started) {
        running_remote_command_count_++;
    } else {
        running_remote_command_count_--;
    }
}

IRemoteAgregator::State IRemoteAgregator::state() const
{
    return state_;
}

void IRemoteAgregator::setStopped()
{
    setState(State::Stopped);
}
