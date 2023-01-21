#include "DisconnectedMidiOutNode.hpp"

#include "imgui.h"
#include "imnodes.h"

#include "PortNameDisplay.hpp"

namespace mc
{

DisconnectedMidiOutNode::DisconnectedMidiOutNode(const std::string&     output_name,
                                                 const PortNameDisplay& port_name_display)
    : m_output_name(output_name), m_port_name_display(&port_name_display)
{
}

void DisconnectedMidiOutNode::render_internal()
{
    ImNodes::BeginNodeTitleBar();
    const auto display_name =
        m_port_name_display->get_port_name(midi::OutputInfo{0, m_output_name});
    ImGui::TextUnformatted(display_name.c_str());
    ImNodes::EndNodeTitleBar();
    ImNodes::BeginInputAttribute(in_id());
    ImGui::TextUnformatted("disconnected");
    ImNodes::EndInputAttribute();
}

} // namespace mc
