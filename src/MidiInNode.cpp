#include "MidiInNode.hpp"

#include <numeric>

#include "imgui.h"
#include "imnodes.h"

#include "NodeSerializer.hpp"
#include "PortNameDisplay.hpp"
#include "midi/InputNode.hpp"
#include "midi/MidiProbe.hpp"

namespace mc
{

MidiInNode::MidiInNode(std::string_view                 input_name,
                       std::shared_ptr<midi::InputNode> midi_input_node,
                       const PortNameDisplay&           port_name_display,
                       const ScaleProvider&             scale_provider)
    : Node(scale_provider, midi_input_node.get()), m_input_name(input_name),
      m_midi_input_node(midi_input_node), m_port_name_display(&port_name_display)
{
    if (m_midi_input_node)
    {
        m_midi_input_node->enable_message_types(m_message_type_mask);
    }
}

void MidiInNode::accept_serializer(nlohmann::json& j, const NodeSerializer& serializer) const
{
    serializer.serialize_node(j, *this);
}

void MidiInNode::render_inspector()
{
    ImGui::SeparatorText(m_port_name_display->get_port_name(m_input_name).c_str());
    midi::MessageTypeMask new_message_type_mask = m_message_type_mask;
    ImGui::Checkbox("Receive SysEx", &new_message_type_mask.m_sysex_enabled);
    ImGui::Checkbox("Receive MIDI Clock", &new_message_type_mask.m_time_enabled);
    ImGui::Checkbox("Receive Active Sensing", &new_message_type_mask.m_sensing_enabled);
    set_message_type_mask(new_message_type_mask);
}

void MidiInNode::render_internal()
{
    check_midi_node_connected();
    const std::string node_title = m_port_name_display->get_port_name(m_input_name);
    Node::begin_output_attribute();
    ImGui::TextUnformatted(node_title.c_str());
    Node::end_output_attribute();
}

void MidiInNode::check_midi_node_connected()
{
    const auto input_opt = midi::MidiProbe::get_valid_input(m_input_name);
    if (input_opt && m_midi_input_node == nullptr)
    {
        m_midi_input_node = std::make_shared<midi::InputNode>(*input_opt);
        m_midi_input_node->enable_message_types(m_message_type_mask);
        for (auto output_connected_node : get_output_connections())
        {
            if (auto node = output_connected_node.lock(); node != nullptr)
            {
                if (auto* midi_node = node->get_midi_node(); midi_node != nullptr)
                {
                    midi::Connection::create(*m_midi_input_node, *midi_node);
                }
            }
        }
    }
    else if (!input_opt && m_midi_input_node != nullptr)
    {
        m_midi_input_node = nullptr;
    }
}

void MidiInNode::set_message_type_mask(midi::MessageTypeMask new_value)
{
    if (m_message_type_mask == new_value)
    {
        return;
    }
    m_message_type_mask = new_value;
    if (m_midi_input_node)
    {
        m_midi_input_node->enable_message_types(new_value);
    }
}

} // namespace mc
