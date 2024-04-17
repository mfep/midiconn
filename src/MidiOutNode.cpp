#include "MidiOutNode.hpp"

#include "imgui.h"
#include "imnodes.h"

#include "NodeSerializer.hpp"
#include "PortNameDisplay.hpp"
#include "midi/OutputNode.hpp"

namespace mc
{

MidiOutNode::MidiOutNode(const midi::OutputInfo&           output_info,
                         std::shared_ptr<midi::OutputNode> midi_output_node,
                         const PortNameDisplay&            port_name_display)
    : m_output_info(output_info), m_midi_output_node(midi_output_node),
      m_port_name_display(&port_name_display)
{
    m_midi_output_node->add_observer(this);
}

MidiOutNode::~MidiOutNode()
{
    m_midi_output_node->remove_observer(this);
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
    ImNodes::BeginNodeTitleBar();
    const std::string node_title = m_port_name_display->get_port_name(m_output_info);
    ImGui::TextUnformatted(node_title.c_str());
    ImNodes::EndNodeTitleBar();
    ImNodes::BeginInputAttribute(in_id());
    m_midi_activity.render();
    ImGui::SameLine();
    ImGui::TextUnformatted("all channels");

    ImNodes::EndInputAttribute();
}

void MidiOutNode::message_received(std::span<const unsigned char> /*message_bytes*/)
{
    m_midi_activity.trigger();
}

} // namespace mc
