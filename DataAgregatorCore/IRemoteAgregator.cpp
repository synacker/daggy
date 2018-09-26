#include "Precompiled.h"
#include "IRemoteAgregator.h"

IRemoteAgregator::IRemoteAgregator(QObject* pParent)
    : QObject(pParent)
    , m_state(State::Stopped)
{
}

bool IRemoteAgregator::isExistsRunningRemoteCommands() const
{
    return runingRemoteCommandsCount() > 0;
}

void IRemoteAgregator::start()
{
    if (m_state == State::Stopped) {
        setState(State::Run);
        startAgregator();
    }
}

void IRemoteAgregator::stop()
{
    if (m_state == State::Run) {
        setState(State::Stopping);
        stopAgregator();
    }
}

void IRemoteAgregator::setState(const IRemoteAgregator::State state)
{
    if (m_state != state) {
        m_state = state;
        emit stateChanged(state);
    }
}

IRemoteAgregator::State IRemoteAgregator::state() const
{
    return m_state;
}

bool IRemoteAgregator::isStoping() const
{
    return m_isStoping;
}
