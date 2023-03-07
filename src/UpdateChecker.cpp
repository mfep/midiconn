#include "UpdateChecker.hpp"

#include "ApplicationName.hpp"
#include "ConfigFile.hpp"
#include "Version.hpp"

#include "SDL2/SDL_misc.h"
#include "cpr/cpr.h"
#include "imgui.h"
#include "nlohmann/json.hpp"
#include "spdlog/spdlog.h"

#include <chrono>
#include <future>

namespace mc
{

UpdateChecker::UpdateChecker(ConfigFile& config) : m_config(&config)
{
    if (config.get_check_updates())
    {
        trigger_check();
    }
}

void UpdateChecker::show_latest_version_message()
{
    if (!m_update_check_finished && m_latest_version_written)
    {
        if (!m_latest_version_result.first)
        {
            ImGui::OpenPopup("Check for latest version");
        }
        m_update_check_finished = true;
    }

    const ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    bool dummy{true};
    if (ImGui::BeginPopupModal(
            "Check for latest version", &dummy, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("A new version of " MIDI_APPLICATION_NAME " is available.");
        ImGui::TextUnformatted("Current version: " MC_SHORT_VERSION);
        ImGui::Text("Latest version: %s", m_latest_version_result.second.c_str());
        bool check_updates = m_config->get_check_updates();
        if (ImGui::Checkbox("Check for updates on startup", &check_updates))
        {
            m_config->set_check_updates(check_updates);
        }
        if (ImGui::Button("Visit website"))
        {
            SDL_OpenURL(MC_WEBSITE_URL);
        }
        ImGui::SameLine();
        if (ImGui::Button("Close"))
        {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

void UpdateChecker::trigger_check()
{
    m_update_check_finished  = false;
    m_latest_version_written = false;
    auto future              = std::async(std::launch::async, [this] {
        using namespace std::chrono_literals;

        std::pair<bool, std::string> result;

        try
        {
            auto response =
                cpr::Get(cpr::Url{"https://gitlab.com/api/v4/projects/32374118/repository/tags"},
                         cpr::Timeout(3s));
            if (response.status_code >= 400)
            {
                spdlog::warn("Could not query GitLab API for the latest version");
                result = {true, std::string()};
            }
            else
            {
                const auto j        = nlohmann::json::parse(response.text);
                const auto tag_name = j.at(0).at("name").get<std::string>();
                const bool is_latest =
                    tag_name == MC_MAJOR_VERSION "." MC_MINOR_VERSION "." MC_PATCH_VERSION;
                result = {is_latest, tag_name};
            }
        }
        catch (std::exception& ex)
        {
            result = {true, std::string()};
        }
        m_latest_version_result  = result;
        m_latest_version_written = true;
    });
    (void)future;
}

bool UpdateChecker::update_check_supported() const
{
    return true;
}

} // namespace mc
