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
                         const PortNameDisplay&            port_name_display,
                         const ScaleProvider&              scale_provider)
    : Node(scale_provider, midi_output_node.get()), m_output_name(output_name),
      m_midi_output_node(midi_output_node), m_port_name_display(&port_name_display)
{
}

void MidiOutNode::accept_serializer(nlohmann::json& j, const NodeSerializer& serializer) const
{
    serializer.serialize_node(j, *this);
}

void MidiOutNode::render_inspector()
{
    ImGui::SeparatorText(m_port_name_display->get_port_name(m_output_name).c_str());
}

void MidiOutNode::render_internal()
{
    check_midi_node_connected();
    const std::string node_title = m_port_name_display->get_port_name(m_output_name);
    Node::begin_input_attribute();
    ImGui::TextUnformatted(node_title.c_str());
    Node::end_input_attribute();
}

void MidiOutNode::check_midi_node_connected()
{
    const auto output_opt = midi::MidiProbe::get_valid_output(m_output_name);
    if (output_opt && m_midi_output_node == nullptr)
    {
        m_midi_output_node = std::make_shared<midi::OutputNode>(*output_opt);
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
        m_midi_output_node = nullptr;
    }
}

} // namespace mc
