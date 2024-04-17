#include "GraphObserver.hpp"

#include <algorithm>

void mc::midi::GraphObserver::message_processed(std::span<const unsigned char> /*message_bytes*/)
{
}

void mc::midi::GraphObserver::message_received(std::span<const unsigned char> /*message_bytes*/)
{
}

void mc::midi::GraphObservable::add_observer(GraphObserver* observer)
{
    auto found_observer = std::find(m_observers.cbegin(), m_observers.cend(), observer);
    if (found_observer == m_observers.cend())
    {
        m_observers.push_back(observer);
    }
}

void mc::midi::GraphObservable::remove_observer(GraphObserver* observer)
{
    auto found_observer = std::find(m_observers.cbegin(), m_observers.cend(), observer);
    if (found_observer != m_observers.cend())
    {
        m_observers.erase(found_observer);
    }
}

void mc::midi::GraphObservable::raise_message_processed(
    std::span<const unsigned char> message_bytes) const
{
    for (auto* observer : m_observers)
    {
        observer->message_processed(message_bytes);
    }
}

void mc::midi::GraphObservable::raise_message_received(
    std::span<const unsigned char> message_bytes) const
{
    for (auto* observer : m_observers)
    {
        observer->message_received(message_bytes);
    }
}
