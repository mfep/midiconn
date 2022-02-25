#include "MidiInNode.hpp"
#include <algorithm>
#include <numeric>
#include "imgui.h"
#include "imnodes.h"
#include "MidiEngine.hpp"

namespace mc
{

MidiInNode::MidiInNode(const midi::InputInfo& input_info, midi::Engine& midi_engine) :
    m_input_info(input_info),
    m_midi_engine(midi_engine)
{
    midi_engine.create(input_info, this);
    auto& map = m_input_sources[input_info.m_id] = {};
    std::iota(map.begin(), map.end(), 0ull);
}

MidiInNode::~MidiInNode()
{
    m_midi_engine.remove(m_input_info, this);
}

void MidiInNode::render_internal()
{
    imnodes::BeginNodeTitleBar();
    ImGui::TextUnformatted(m_input_info.m_name.c_str());
    imnodes::EndNodeTitleBar();
    imnodes::BeginOutputAttribute(out_id());

    constexpr float r_component = 0;
    constexpr float g_component = 1;
    constexpr float b_component = 0;
    constexpr double fade_time_ms = 1000;

    const auto ms_since_last_message = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now() - m_last_message_received).count();
    const double percent = 1 - std::min(1., ms_since_last_message / fade_time_ms);
    const auto color = ImVec4(0, 1, 0, percent);
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
    imnodes::EndOutputAttribute();
}

void MidiInNode::message_received(size_t id, std::vector<unsigned char>& message_bytes)
{
    m_last_message_received = std::chrono::system_clock::now();
}

}
