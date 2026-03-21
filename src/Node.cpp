#include "Node.hpp"

#include "ImGuiUtils.hpp"
#include "ScaleProvider.hpp"
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

Node::Node(const ScaleProvider& scale_provider, midi::Node* midi_node)
    : m_scale_provider(&scale_provider), m_id(sm_next_id++), m_midi_node(midi_node)
{
    if (m_midi_node)
    {
        m_midi_node->add_observer(this);
    }
}

Node::~Node()
{
    if (m_midi_node)
    {
        m_midi_node->remove_observer(this);
    }
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

    ImNodes::SnapNodeToGrid(m_id);
    const auto text_size = ImGui::CalcTextSize("a");
    ImNodes::PushStyleVar(ImNodesStyleVar_NodePadding,
                          {m_scale_provider->get_scale_value() * 15.F,
                           (m_scale_provider->get_scale_value() * 60.F - text_size.y) * 0.5F});

    push_style();
    ImNodes::BeginNode(m_id);
    render_internal();
    ImNodes::EndNode();
    pop_style();
    ImNodes::PopStyleVar(1);
    ImNodes::PushColorStyle(ImNodesCol_Link, get_pin_color(m_last_output_activity));
    for (const auto& [link_id, node] : m_output_connections)
    {
        ImNodes::Link(link_id, out_id(), std::shared_ptr(node)->in_id());
    }
    ImNodes::PopColorStyle();
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

uint32_t Node::get_pin_color(time_point_t last_activity)
{
    constexpr double fade_time_ms = 1000;
    const auto       ms_since_last_message =
        std::chrono::duration_cast<std::chrono::milliseconds>(clock_t::now() - last_activity)
            .count();
    const double percent = 1 - std::min(1., ms_since_last_message / fade_time_ms);

    const auto base_color =
        ImGui::ColorConvertU32ToFloat4(ImNodes::GetStyle().Colors[ImNodesCol_Pin]);
    const ImVec4 target_color{79.F / 255, 249.F / 255, 58.F / 255, 1.F};
    const auto   current_color =
        ImGui::ColorConvertFloat4ToU32(utils::color_lerp(base_color, target_color, percent));
    return current_color;
}

void Node::message_processed(std::span<const unsigned char> /*message_bytes*/)
{
    m_last_output_activity = clock_t::now();
}

void Node::message_received(std::span<const unsigned char> /*message_bytes*/)
{
    m_last_input_activity = clock_t::now();
}

void Node::begin_input_attribute() const
{
    ImNodes::PushColorStyle(ImNodesCol_Pin, get_pin_color(m_last_input_activity));
    ImNodes::BeginInputAttribute(in_id());
}

void Node::end_input_attribute() const
{
    ImNodes::EndInputAttribute();
    ImNodes::PopColorStyle();
}

void Node::begin_output_attribute() const
{
    ImNodes::PushColorStyle(ImNodesCol_Pin, get_pin_color(m_last_output_activity));
    ImNodes::BeginOutputAttribute(out_id());
}

void Node::end_output_attribute() const
{
    ImNodes::EndOutputAttribute();
    ImNodes::PopColorStyle();
}

} // namespace mc
