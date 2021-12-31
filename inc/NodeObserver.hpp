#pragma once
#include <vector>
#include "MidiEngine.hpp"

namespace mc::display
{

class NodeObserver
{
public:
    virtual ~NodeObserver() = default;
    
    virtual void node_created(const midi::InputInfo& input_info) {};
    virtual void node_created(const midi::OutputInfo& output_info) {};
    virtual void node_destroyed(const midi::InputInfo& input_info) {};
    virtual void node_destroyed(const midi::OutputInfo& output_info) {};
    virtual void link_created(const midi::InputInfo& input_info, const midi::OutputInfo& output_info) {};
    virtual void link_destroyed(const midi::InputInfo& input_info, const midi::OutputInfo& output_info) {};
};

class NodeObservable
{
public:
    virtual ~NodeObservable() = default;

    void add_observer(NodeObserver* observer);
    void delete_observer(NodeObserver* observer);

protected:
    void raise_node_created(const midi::InputInfo& input_info) const;
    void raise_node_created(const midi::OutputInfo& output_info) const;
    void raise_node_destroyed(const midi::InputInfo& input_info) const;
    void raise_node_destroyed(const midi::OutputInfo& output_info) const;
    void raise_link_created(const midi::InputInfo& input_info, const midi::OutputInfo& output_info) const;
    void raise_link_destroyed(const midi::InputInfo& input_info, const midi::OutputInfo& output_info) const;

private:
    std::vector<NodeObserver*> m_observers;
};

}
