#include "WelcomeWindow.hpp"

#include "ApplicationName.hpp"
#include "ConfigFile.hpp"

#include "imgui.h"

namespace mc
{

WelcomeWindow::WelcomeWindow(ConfigFile& config)
    : m_config(&config), m_enabled(config.get_show_welcome())
{
}

void WelcomeWindow::render()
{
    if (m_enabled)
    {
        ImGui::OpenPopup("Welcome");
        m_enabled = false;
    }
    const ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    if (ImGui::BeginPopupModal("Welcome", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::TextUnformatted("Welcome to " MIDI_APPLICATION_NAME);
        bool show_on_startup = m_config->get_show_welcome();
        if (ImGui::Checkbox("Show this window on application startup", &show_on_startup))
        {
            m_config->set_show_welcome(show_on_startup);
        }
        if (ImGui::Button("New preset - empty"))
        {
            ImGui::CloseCurrentPopup();
        }
        if (ImGui::Button("New preset - add all MIDI devices"))
        {
            ImGui::CloseCurrentPopup();
        }
        const auto last_preset_opt = m_config->get_last_preset_path();
        ImGui::BeginDisabled(!last_preset_opt);
        if (ImGui::Button("Open last preset"))
        {
            ImGui::CloseCurrentPopup();
        }
        if (last_preset_opt && ImGui::IsItemHovered())
        {
            ImGui::SetTooltip("%s", last_preset_opt.value().c_str());
        }
        ImGui::EndDisabled();
        ImGui::EndPopup();
    }
}

} // namespace mc
