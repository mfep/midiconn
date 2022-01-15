#include "Node.hpp"
#include <algorithm>
#include "imgui.h"
#include "imnodes.h"

namespace mc
{

Node::Node() :
    m_id(sm_next_id++)
{
}

void Node::render()
{
    auto purge = [](std::map<int, node_ptr>& connections) -> bool
    {
        bool changed = false;
        for (auto it = connections.begin(); it != connections.end();)
        {
            if (it->second.expired())
            {
                changed = true;
                it = connections.erase(it);
            }
            else
            {
                ++it;
            }
        }
        return changed;
    };
    const bool input_changed = purge(m_input_connections);
    purge(m_output_connections);
    if (input_changed)
    {
        update_sources_from_inputs();
    }

    imnodes::BeginNode(m_id);
    render_internal();
    imnodes::EndNode();

    for (const auto&[link_id, node] : m_output_connections)
    {
        imnodes::Link(link_id, out_id(), std::shared_ptr(node)->in_id());
    }
}

void Node::connect_output(node_ptr to_node, node_ptr this_node)
{
    auto link_id = sm_next_link_id++;
    m_output_connections[link_id] = to_node;
    std::shared_ptr(to_node)->connect_input(this_node, link_id);
}

void Node::disconnect_output(int link_id)
{
    if (auto ptr = m_output_connections[link_id].lock(); ptr != nullptr)
    {
        ptr->disconnect_input(link_id);
    }
    m_output_connections.erase(link_id);
}

midi::channel_map Node::transform_channel_map(const midi::channel_map& in_map)
{
    return in_map;
}

void Node::update_outputs_with_sources()
{
    for (auto it = m_output_connections.begin(); it != m_output_connections.end(); ++it)
    {
        auto node_ptr = it->second.lock();
        if (node_ptr == nullptr)
        {
            continue;
        }
        node_ptr->update_sources_from_inputs(it->first);
    }
}

void Node::connect_input(node_ptr from_node, int link_id)
{
    m_input_connections[link_id] = from_node;
    update_sources_from_inputs(link_id);
}

void Node::disconnect_input(int link_id)
{
    m_input_connections.erase(link_id);
    update_sources_from_inputs();
}

void Node::update_sources_from_inputs(int new_link_id)
{
    m_sources.clear();
    if (new_link_id >= 0)
    {
        const auto& base_sources = m_input_connections.at(new_link_id).lock()->get_sources();
        for (auto&[input_id, map] : base_sources)
        {
            m_sources[input_id] = transform_channel_map(map);
        }
    }
    for (auto it = m_input_connections.begin(); it != m_input_connections.end(); ++it)
    {
        auto node_ptr = it->second.lock();
        if (node_ptr == nullptr || it->first == new_link_id)
        {
            continue;
        }
        midi_sources new_sources;
        bool has_conflicting_input{};
        for (auto&[input_id, map] : node_ptr->get_sources())
        {
            auto found_id_it = m_sources.find(input_id);
            if (found_id_it != m_sources.end())
            {
                node_ptr->m_output_connections.erase(it->first);
                has_conflicting_input = true;
                break;
            }
            new_sources[input_id] = transform_channel_map(map);
        }
        if (!has_conflicting_input)
        {
            m_sources.merge(new_sources);
        }
    }
    
    update_outputs_with_sources();
}

}
