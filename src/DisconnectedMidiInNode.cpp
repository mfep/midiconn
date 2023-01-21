#include "DisconnectedMidiInNode.hpp"

#include "imgui.h"
#include "imnodes.h"

#include "PortNameDisplay.hpp"

namespace mc
{

DisconnectedMidiInNode::DisconnectedMidiInNode(const std::string&     input_name,
                                               const PortNameDisplay& port_name_display)
    : m_input_name(input_name), m_port_name_display(&port_name_display)
{
}

void DisconnectedMidiInNode::render_internal()
{
    ImNodes::BeginNodeTitleBar();
    const auto display_name = m_port_name_display->get_port_name(midi::InputInfo{0, m_input_name});
    ImGui::TextUnformatted(display_name.c_str());
    ImNodes::EndNodeTitleBar();
    ImNodes::BeginOutputAttribute(out_id());
    ImGui::TextUnformatted("disconnected");
    ImNodes::EndOutputAttribute();
}

} // namespace mc
