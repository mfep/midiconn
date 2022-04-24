#include "DisconnectedMidiInNode.hpp"

#include "imgui.h"
#include "imnodes.h"

#include "NodeSerializer.hpp"

namespace mc
{

DisconnectedMidiInNode::DisconnectedMidiInNode(const std::string& input_name) :
    m_input_name(input_name)
{
}

void DisconnectedMidiInNode::accept_serializer(nlohmann::json& j, const NodeSerializer& serializer) const
{
    serializer.serialize_node(j, *this);
}

void DisconnectedMidiInNode::render_internal()
{
    ImNodes::BeginNodeTitleBar();
    ImGui::TextUnformatted(m_input_name.c_str());
    ImNodes::EndNodeTitleBar();
    ImNodes::BeginOutputAttribute(out_id());
    ImGui::TextUnformatted("disconnected");
    ImNodes::EndOutputAttribute();
}

}
