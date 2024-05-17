#include "MidiOutNode.hpp"

#include "imgui.h"
#include "imnodes.h"

#include "NodeSerializer.hpp"
#include "PortNameDisplay.hpp"
#include "midi/MidiProbe.hpp"
#include "midi/OutputNode.hpp"

namespace mc
{

MidiOutNode::MidiOutNode(std::string_view                  output_name,
                         std::shared_ptr<midi::OutputNode> midi_output_node,
                         const PortNameDisplay&            port_name_display)
    : m_output_name(output_name), m_midi_output_node(midi_output_node),
      m_port_name_display(&port_name_display)
{
    if (m_midi_output_node)
    {
        m_midi_output_node->add_observer(this);
    }
}

MidiOutNode::~MidiOutNode()
{
    if (m_midi_output_node)
    {
        m_midi_output_node->remove_observer(this);
    }
}

void MidiOutNode::accept_serializer(nlohmann::json& j, const NodeSerializer& serializer) const
{
    serializer.serialize_node(j, *this);
}

midi::Node* MidiOutNode::get_midi_node()
{
    return m_midi_output_node.get();
}

void MidiOutNode::render_internal()
{
    check_midi_node_connected();

    ImNodes::BeginNodeTitleBar();
    const std::string node_title = m_port_name_display->get_port_name(m_output_name);
    ImGui::TextUnformatted(node_title.c_str());
    ImNodes::EndNodeTitleBar();
    ImNodes::BeginInputAttribute(in_id());

    if (m_midi_output_node)
    {
        m_midi_activity.render();
        ImGui::SameLine();
        ImGui::TextUnformatted("all channels");
    }
    else
    {
        ImGui::TextUnformatted("disconnected");
    }

    ImNodes::EndInputAttribute();
}

void MidiOutNode::message_received(std::span<const unsigned char> /*message_bytes*/)
{
    m_midi_activity.trigger();
}

void MidiOutNode::check_midi_node_connected()
{
    const auto output_opt = midi::MidiProbe::get_valid_output(m_output_name);
    if (output_opt && m_midi_output_node == nullptr)
    {
        m_midi_output_node = std::make_shared<midi::OutputNode>(*output_opt);
        m_midi_output_node->add_observer(this);
        for (auto input_connected_node : get_input_connections())
        {
            if (auto node = input_connected_node.lock(); node != nullptr)
            {
                if (auto* midi_node = node->get_midi_node(); midi_node != nullptr)
                {
                    midi::Connection::create(*midi_node, *m_midi_output_node);
                }
            }
        }
    }
    else if (!output_opt && m_midi_output_node != nullptr)
    {
        m_midi_output_node->remove_observer(this);
        m_midi_output_node = nullptr;
    }
}

} // namespace mc
