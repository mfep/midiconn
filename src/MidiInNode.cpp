#include "MidiInNode.hpp"

#include <numeric>

#include "imgui.h"
#include "imnodes.h"

#include "Intl.hpp"
#include "NodeSerializer.hpp"
#include "PortNameDisplay.hpp"
#include "midi/InputNode.hpp"
#include "midi/MidiProbe.hpp"

namespace mc
{

MidiInNode::MidiInNode(std::string_view                 input_name,
                       std::shared_ptr<midi::InputNode> midi_input_node,
                       const PortNameDisplay&           port_name_display)
    : m_input_name(input_name), m_midi_input_node(midi_input_node),
      m_port_name_display(&port_name_display)
{
    if (m_midi_input_node)
    {
        m_midi_input_node->enable_message_types(m_message_type_mask);
        m_midi_input_node->add_observer(this);
    }
}

MidiInNode::~MidiInNode()
{
    if (m_midi_input_node)
    {
        m_midi_input_node->remove_observer(this);
    }
}

void MidiInNode::accept_serializer(nlohmann::json& j, const NodeSerializer& serializer) const
{
    serializer.serialize_node(j, *this);
}

midi::Node* MidiInNode::get_midi_node()
{
    return m_midi_input_node.get();
}

void MidiInNode::render_internal()
{
    check_midi_node_connected();

    ImNodes::BeginNodeTitleBar();
    const std::string node_title = m_port_name_display->get_port_name(m_input_name);
    ImGui::TextUnformatted(node_title.c_str());
    ImNodes::EndNodeTitleBar();
    ImNodes::BeginOutputAttribute(out_id());
    if (m_midi_input_node)
    {
        m_midi_activity.render();
        ImGui::SameLine();
        // Translators: Label of the connection pin in the MIDI input node
        ImGui::TextUnformatted(gettext("all channels"));
    }
    else
    {
        // Translators: Label of the connection pin in the MIDI input node when the device is not
        // available
        ImGui::TextUnformatted(gettext("disconnected"));
    }
    ImNodes::EndOutputAttribute();

    if (m_midi_input_node == nullptr)
    {
        return;
    }

    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4{});
    ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4{});
    // Translators: The label of the dropdown in the MIDI input node
    if (ImGui::TreeNode(gettext("Advanced")))
    {
        midi::MessageTypeMask new_message_type_mask = m_message_type_mask;
        ImGui::Checkbox(gettext("Receive SysEx"), &new_message_type_mask.m_sysex_enabled);
        ImGui::Checkbox(gettext("Receive MIDI Clock"), &new_message_type_mask.m_time_enabled);
        ImGui::Checkbox(gettext("Receive Active Sensing"),
                        &new_message_type_mask.m_sensing_enabled);
        set_message_type_mask(new_message_type_mask);
        ImGui::TreePop();
    }
    ImGui::PopStyleColor(2);
}

void MidiInNode::message_processed(std::span<const unsigned char> /*message_bytes*/)
{
    m_midi_activity.trigger();
}

void MidiInNode::check_midi_node_connected()
{
    const auto input_opt = midi::MidiProbe::get_valid_input(m_input_name);
    if (input_opt && m_midi_input_node == nullptr)
    {
        m_midi_input_node = std::make_shared<midi::InputNode>(*input_opt);
        m_midi_input_node->enable_message_types(m_message_type_mask);
        m_midi_input_node->add_observer(this);
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
        m_midi_input_node->remove_observer(this);
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
