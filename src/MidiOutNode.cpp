#include "MidiOutNode.hpp"

#include "IconsForkAwesome.h"
#include "imgui.h"
#include "imnodes.h"

#include "Intl.hpp"
#include "NodeSerializer.hpp"
#include "PortNameDisplay.hpp"
#include "midi/MidiProbe.hpp"
#include "midi/OutputNode.hpp"

namespace mc
{

MidiOutNode::MidiOutNode(std::string_view                  output_name,
                         std::shared_ptr<midi::OutputNode> midi_output_node,
                         const PortNameDisplay&            port_name_display,
                         const ScaleProvider&              scale_provider)
    : Node(scale_provider), m_output_name(output_name), m_midi_output_node(midi_output_node),
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

void MidiOutNode::render_inspector()
{
    ImGui::SeparatorText(m_port_name_display->get_port_name(m_output_name).c_str());
}

midi::Node* MidiOutNode::get_midi_node()
{
    return m_midi_output_node.get();
}

void MidiOutNode::render_internal()
{
    check_midi_node_connected();
    const std::string node_title = m_port_name_display->get_port_name(m_output_name);
    ImNodes::BeginInputAttribute(in_id());
    m_midi_activity.render();
    ImGui::SameLine();
    ImGui::Text(ICON_FK_ARROW_CIRCLE_O_RIGHT " %s", node_title.c_str());
    ImNodes::EndOutputAttribute();
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
