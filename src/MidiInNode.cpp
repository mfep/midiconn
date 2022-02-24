#include "MidiInNode.hpp"
#include <algorithm>
#include <numeric>
#include "imgui.h"
#include "imnodes.h"
#include "MidiEngine.hpp"
#include "spdlog/spdlog.h"

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
    using namespace std::literals;
    imnodes::BeginNodeTitleBar();
    ImGui::TextUnformatted(m_input_info.m_name.c_str());
    imnodes::EndNodeTitleBar();
    imnodes::BeginOutputAttribute(out_id());
    const bool active = std::chrono::system_clock::now() - m_last_message_received < 1s;
    if (active)
    {
        const auto color = ImVec4(0, 255, 0, 255);
        ImGui::PushStyleColor(ImGuiCol_Button, color);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, color);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, color);
    }
    ImGui::BeginDisabled();
    ImGui::Button("  ");
    ImGui::EndDisabled();
    if (active)
    {
        ImGui::PopStyleColor(3);
    }
    ImGui::SameLine();
    ImGui::TextUnformatted("all channels");
    imnodes::EndOutputAttribute();
}

void MidiInNode::message_received(size_t id, std::vector<unsigned char>& message_bytes)
{
    m_last_message_received = std::chrono::system_clock::now();
}

}
