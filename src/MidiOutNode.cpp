#include "MidiOutNode.hpp"

#include "imgui.h"
#include "imnodes.h"

#include "NodeSerializer.hpp"
#include "PortNameDisplay.hpp"
#include "midi/MidiEngine.hpp"

namespace mc
{

MidiOutNode::MidiOutNode(const midi::OutputInfo& output_info,
                         midi::Engine&           midi_engine,
                         const PortNameDisplay&  port_name_display)
    : m_output_info(output_info), m_midi_engine(&midi_engine),
      m_port_name_display(&port_name_display)
{
    m_midi_engine->create(output_info, this);
}

MidiOutNode::~MidiOutNode()
{
    m_midi_engine->remove(m_output_info, this);
}

void MidiOutNode::accept_serializer(nlohmann::json& j, const NodeSerializer& serializer) const
{
    serializer.serialize_node(j, *this);
}

void MidiOutNode::render_internal()
{
    ImNodes::BeginNodeTitleBar();
    const std::string node_title = m_port_name_display->get_port_name(m_output_info);
    ImGui::TextUnformatted(node_title.c_str());
    ImNodes::EndNodeTitleBar();
    ImNodes::BeginInputAttribute(in_id());
    constexpr double fade_time_ms          = 1000;
    const auto       ms_since_last_message = std::chrono::duration_cast<std::chrono::milliseconds>(
                                           std::chrono::system_clock::now() - m_last_message_sent)
                                           .count();
    const double percent = 1 - std::min(1., ms_since_last_message / fade_time_ms);
    const auto   color   = ImVec4(0, 1, 0, percent);
    ImGui::PushStyleColor(ImGuiCol_Button, color);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, color);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, color);
    ImGui::PushStyleVar(ImGuiStyleVar_DisabledAlpha, 1);
    ImGui::BeginDisabled();
    ImGui::Button("  ");
    ImGui::EndDisabled();
    ImGui::PopStyleVar();
    ImGui::PopStyleColor(3);
    ImGui::SameLine();
    ImGui::TextUnformatted("all channels");

    ImNodes::EndInputAttribute();
}

void MidiOutNode::update_outputs_with_sources()
{
    for (auto& [id, map] : m_previous_sources)
    {
        m_midi_engine->disconnect(id, m_output_info.m_id);
    }
    for (auto& [id, map] : m_input_sources)
    {
        m_midi_engine->connect(id, m_output_info.m_id, map);
    }
    m_previous_sources = m_input_sources;
}

void MidiOutNode::message_sent(size_t /*id*/, const std::vector<unsigned char>& /*message_bytes*/)
{
    m_last_message_sent = std::chrono::system_clock::now();
}

} // namespace mc
