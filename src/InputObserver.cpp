#include "InputObserver.hpp"

#include <algorithm>

namespace mc::midi
{

void InputObservable::add_observer(InputObserver* observer)
{
    auto found_observer = std::find(m_observers.cbegin(), m_observers.cend(), observer);
    if (found_observer == m_observers.cend())
    {
        m_observers.push_back(observer);
    }
}

void InputObservable::remove_observer(InputObserver* observer)
{
    auto found_observer = std::find(m_observers.cbegin(), m_observers.cend(), observer);
    if (found_observer != m_observers.cend())
    {
        m_observers.erase(found_observer);
    }
}

void InputObservable::raise_message_received(
    size_t id,
    std::vector<unsigned char>& message_bytes) const
{
    for (auto* observer : m_observers)
    {
        observer->message_received(id, message_bytes);
    }
}

}
