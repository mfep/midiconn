#include "NodeObserver.hpp"
#include <algorithm>

namespace mc::display
{

void NodeObservable::add_observer(NodeObserver* observer)
{
    if (m_observers.cend() == std::find(m_observers.cbegin(), m_observers.cend(), observer))
    {
        m_observers.push_back(observer);
    }
}

void NodeObservable::delete_observer(NodeObserver* observer)
{
    auto observer_it = std::find(m_observers.begin(), m_observers.end(), observer);
    if (observer_it != m_observers.end())
    {
        m_observers.erase(observer_it);
    }
}

void NodeObservable::raise_node_created(const midi::InputInfo& input_info) const
{
    for (auto* observer : m_observers)
    {
        observer->node_created(input_info);
    }
}

void NodeObservable::raise_node_created(const midi::OutputInfo& output_info) const
{
    for (auto* observer : m_observers)
    {
        observer->node_created(output_info);
    }
}

void NodeObservable::raise_node_destroyed(const midi::InputInfo& input_info) const
{
    for (auto* observer : m_observers)
    {
        observer->node_destroyed(input_info);
    }
}

void NodeObservable::raise_node_destroyed(const midi::OutputInfo& output_info) const
{
    for (auto* observer : m_observers)
    {
        observer->node_destroyed(output_info);
    }
}

void NodeObservable::raise_link_created(const midi::InputInfo& input_info, const midi::OutputInfo& output_info) const
{
    for (auto* observer : m_observers)
    {
        observer->link_created(input_info, output_info);
    }
}

void NodeObservable::raise_link_destroyed(const midi::InputInfo& input_info, const midi::OutputInfo& output_info) const
{
    for (auto* observer : m_observers)
    {
        observer->link_destroyed(input_info, output_info);
    }
}

}
