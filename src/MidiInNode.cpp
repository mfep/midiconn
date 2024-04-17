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

MidiInNode::MidiInNode(const midi::InputInfo&           input_info,
                       std::shared_ptr<midi::InputNode> midi_input_node,
                       const PortNameDisplay&           port_name_display)
    : m_input_info(input_info), m_midi_input_node(midi_input_node),
      m_port_name_display(&port_name_display)
{
    m_midi_input_node->add_observer(this);
}

MidiInNode::~MidiInNode()
{
    m_midi_input_node->remove_observer(this);
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
    ImNodes::BeginNodeTitleBar();
    const std::string node_title = m_port_name_display->get_port_name(m_input_info);
    ImGui::TextUnformatted(node_title.c_str());
    ImNodes::EndNodeTitleBar();
    ImNodes::BeginOutputAttribute(out_id());
    m_midi_activity.render();
    ImGui::SameLine();

    ImGui::TextUnformatted("all channels");
    ImNodes::EndOutputAttribute();
}

void MidiInNode::message_processed(std::span<const unsigned char> /*message_bytes*/)
{
    m_midi_activity.trigger();
}

} // namespace mc
