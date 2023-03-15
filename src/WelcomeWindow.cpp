#include "WelcomeWindow.hpp"

#include "ApplicationName.hpp"
#include "ConfigFile.hpp"
#include "UpdateChecker.hpp"
#include "Utils.hpp"
#include "Version.hpp"

#include "imgui.h"
#include "SDL2/SDL_misc.h"

namespace mc
{

WelcomeWindow::WelcomeWindow(ConfigFile& config, UpdateChecker& update_checker)
    : m_config(&config), m_update_checker(&update_checker), m_enabled(config.get_show_welcome())
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
    if (ImGui::BeginPopup("Welcome", ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::TextUnformatted("Welcome to " MIDI_APPLICATION_NAME " " MC_SHORT_VERSION);
        const auto latest_version = m_update_checker->get_latest_version();
        std::visit(utils::overloads{
                       [](const UpdateChecker::StatusNotSupported&) {},
                       [](const UpdateChecker::StatusFetching&) {
                           ImGui::TextUnformatted("Fetching latest " MIDI_APPLICATION_NAME
                                                  " version number...");
                       },
                       [](const UpdateChecker::StatusError& error) {
                           ImGui::TextUnformatted("Error fetching latest " MIDI_APPLICATION_NAME
                                                  " version number");
                           if (!error.message.empty() && ImGui::IsItemHovered())
                               ImGui::SetTooltip("%s", error.message.c_str());
                       },
                       [](const UpdateChecker::StatusFetched& fetched) {
                           if (fetched.is_latest_version)
                           {
                               ImGui::TextUnformatted("Congratulations! You are using the latest "
                                                      "version of " MIDI_APPLICATION_NAME);
                           }
                           else
                           {
                               ImGui::Text("A new version of " MIDI_APPLICATION_NAME
                                           " is available: %s",
                                           fetched.latest_version_name.c_str());
                                ImGui::SameLine();
                                if (ImGui::Button("Visit website"))
                                {
                                    SDL_OpenURL(MC_WEBSITE_URL);
                                }
                           }
                       },
                   },
                   latest_version);

        bool show_on_startup = m_config->get_show_welcome();
        if (ImGui::Checkbox("Show this window on application startup", &show_on_startup))
        {
            m_config->set_show_welcome(show_on_startup);
        }
        if (ImGui::Button("New preset - empty"))
        {
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("New preset - add all MIDI devices"))
        {
            ImGui::CloseCurrentPopup();
        }
        const auto last_preset_opt = m_config->get_last_preset_path();
        ImGui::BeginDisabled(!last_preset_opt);
        ImGui::SameLine();
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
