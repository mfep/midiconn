#include "Node.hpp"

#include "midi/MidiGraph.hpp"

#include "imgui.h"
#include "imnodes.h"

namespace mc
{
namespace
{

std::vector<Node::node_ptr> get_connections(const std::map<int, Node::node_ptr>& node_map)
{
    std::vector<Node::node_ptr> connections;
    for (auto [link_id, other_node_ptr] : node_map)
    {
        connections.push_back(other_node_ptr);
    }
    return connections;
}

} // namespace

Node::Node() : m_id(sm_next_id++)
{
}

void Node::render()
{
    auto purge = [](std::map<int, node_ptr>& connections) {
        for (auto it = connections.begin(); it != connections.end();)
        {
            if (it->second.expired())
            {
                it = connections.erase(it);
            }
            else
            {
                ++it;
            }
        }
    };
    purge(m_input_connections);
    purge(m_output_connections);

    push_style();
    ImNodes::BeginNode(m_id);
    render_internal();
    ImNodes::EndNode();
    pop_style();

    for (const auto& [link_id, node] : m_output_connections)
    {
        ImNodes::Link(link_id, out_id(), std::shared_ptr(node)->in_id());
    }
}

void Node::connect_output(node_ptr to_node, node_ptr this_node)
{
    auto link_id                  = sm_next_link_id++;
    m_output_connections[link_id] = to_node;
    to_node.lock()->connect_input(this_node, link_id);
}

void Node::disconnect_output(int link_id)
{
    if (auto ptr = m_output_connections[link_id].lock(); ptr != nullptr)
    {
        ptr->disconnect_input(link_id);
    }
    m_output_connections.erase(link_id);
}

std::vector<Node::node_ptr> Node::get_output_connections() const
{
    return get_connections(m_output_connections);
}

std::vector<Node::node_ptr> Node::get_input_connections() const
{
    return get_connections(m_input_connections);
}

void Node::connect_input(node_ptr from_node, int link_id)
{
    m_input_connections[link_id] = from_node;
    auto* sender_midi_node       = from_node.lock()->get_midi_node();
    auto* receiver_midi_node     = get_midi_node();
    if (sender_midi_node && receiver_midi_node)
    {
        midi::Connection::create(*sender_midi_node, *receiver_midi_node);
    }
}

void Node::disconnect_input(int link_id)
{
    auto* sender_midi_node   = m_input_connections.at(link_id).lock()->get_midi_node();
    auto* receiver_midi_node = get_midi_node();
    if (sender_midi_node && receiver_midi_node)
    {
        midi::Connection::disconnect(*sender_midi_node, *receiver_midi_node);
    }
    m_input_connections.erase(link_id);
}

} // namespace mc
