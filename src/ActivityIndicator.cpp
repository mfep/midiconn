#include "ActivityIndicator.hpp"

#include "imgui.h"

#include <algorithm>

void mc::ActivityIndicator::render() const
{
    constexpr double fade_time_ms          = 1000;
    const auto       ms_since_last_message = std::chrono::duration_cast<std::chrono::milliseconds>(
                                           std::chrono::system_clock::now() - m_last_trigger)
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
}

void mc::ActivityIndicator::trigger()
{
    m_last_trigger = std::chrono::system_clock::now();
}
