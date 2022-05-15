#include "DisconnectedMidiOutNode.hpp"

#include "imgui.h"
#include "imnodes.h"

#include "NodeSerializer.hpp"

namespace mc
{

DisconnectedMidiOutNode::DisconnectedMidiOutNode(const std::string& output_name)
    : m_output_name(output_name)
{
}

void DisconnectedMidiOutNode::render_internal()
{
    ImNodes::BeginNodeTitleBar();
    ImGui::TextUnformatted(m_output_name.c_str());
    ImNodes::EndNodeTitleBar();
    ImNodes::BeginInputAttribute(in_id());
    ImGui::TextUnformatted("disconnected");
    ImNodes::EndInputAttribute();
}

} // namespace mc
