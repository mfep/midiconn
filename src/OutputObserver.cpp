#include "OutputObserver.hpp"

#include <algorithm>

namespace mc::midi
{

void OutputObservable::add_observer(OutputObserver* observer)
{
    auto found_observer = std::find(m_observers.cbegin(), m_observers.cend(), observer);
    if (found_observer == m_observers.cend())
    {
        m_observers.push_back(observer);
    }
}

void OutputObservable::remove_observer(OutputObserver* observer)
{
    auto found_observer = std::find(m_observers.cbegin(), m_observers.cend(), observer);
    if (found_observer != m_observers.cend())
    {
        m_observers.erase(found_observer);
    }
}

void OutputObservable::raise_message_sent(
    size_t id,
    const std::vector<unsigned char>& message_bytes) const
{
    for (auto* observer : m_observers)
    {
        observer->message_sent(id, message_bytes);
    }
}

}
