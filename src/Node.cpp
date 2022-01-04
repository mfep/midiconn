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
    auto purge = [](std::map<int, node_ptr>& connections)
    {
        for (auto it = connections.begin(); it != connections.end(); ++it)
        {
            if (it->second.expired())
            {
                connections.erase(it);
            }
        }
    };
    // ToDo
    purge(m_input_connections);
    purge(m_output_connections);

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

Node::channel_map Node::transform_channel_map(const channel_map& in_map)
{
    return in_map;
}

void Node::update_outputs()
{
    for (auto it = m_output_connections.begin(); it != m_output_connections.end(); ++it)
    {
        auto node_ptr = it->second.lock();
        if (node_ptr == nullptr)
        {
            continue;
        }
        node_ptr->update_inputs(it->first);
    }
}

void Node::connect_input(node_ptr from_node, int link_id)
{
    m_input_connections[link_id] = from_node;
    update_inputs(link_id);
}

void Node::disconnect_input(int link_id)
{
    m_input_connections.erase(link_id);
    update_inputs();
}

void Node::update_inputs(int new_link_id)
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
        for (auto&[input_id, map] : node_ptr->get_sources())
        {
            auto found_id_it = m_sources.find(input_id);
            if (found_id_it != m_sources.end())
            {
                node_ptr->m_output_connections.erase(it->first);
                break;
            }
            new_sources[input_id] = transform_channel_map(map);
        }
        m_sources.merge(new_sources);
    }
    update_outputs();
}

}
