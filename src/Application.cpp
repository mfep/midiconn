#include "Application.hpp"
#include <fstream>
#include "imgui.h"
#include <SDL2/SDL.h>
#include <nlohmann/json.hpp>
#include "portable-file-dialogs.h"
#include "Licenses.hpp"
#include "MidiEngine.hpp"
#include "NodeEditor.hpp"
#include "Version.hpp"

namespace
{

bool ends_with_dot_json(const std::string& path)
{
    std::string lower_path;
    std::transform(path.begin(), path.end(), std::back_inserter(lower_path),
        [](const auto& ch) { return std::tolower(ch); });
    return lower_path.size() > 5
        && 0 == lower_path.compare(lower_path.size() - 5, 5, ".json");
}

}

namespace mc::display
{

Application::Application() :
    m_is_done(false),
    m_midi_engine(std::make_unique<midi::Engine>()),
    m_node_editor(std::make_unique<NodeEditor>(*m_midi_engine))
{
}

Application::~Application() = default;

void Application::render()
{
    auto* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::Begin("Another Window", nullptr,
        ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoSavedSettings
        | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_MenuBar);

    render_main_menu();
    try
    {
        m_node_editor->render();
    }
    catch(std::exception& ex)
    {
        std::cout << "Error: " << ex.what() << std::endl;
    }

    ImGui::End();
    ImGui::ShowDemoWindow();
}

void Application::render_main_menu()
{
    bool open_about_popup = false;
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Export preset"))
            {
                auto save_path = pfd::save_file("Export preset", ".", { "JSON files (*.json)", "*.json" }).result();
                if (!ends_with_dot_json(save_path))
                {
                    save_path += ".json";
                }
                nlohmann::json j;
                m_node_editor->to_json(j);
                std::ofstream ofs(save_path);
                ofs << j << std::endl;
            }
            if (ImGui::MenuItem("Exit"))
            {
                m_is_done = true;
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Help"))
        {
            if (ImGui::MenuItem("About"))
            {
                open_about_popup = true;
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
    if (open_about_popup)
    {
        ImGui::OpenPopup("About " MIDI_APPLICATION_NAME);
    }

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSizeConstraints({600, 0}, {600, 400});
    if (ImGui::BeginPopupModal("About " MIDI_APPLICATION_NAME, NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::TextUnformatted(MC_MAJOR_VERSION "." MC_MINOR_VERSION "." MC_PATCH_VERSION "." MC_BUILD_NUMBER);
        ImGui::TextUnformatted(MC_COMMIT_HASH);
        ImGui::TextUnformatted(MC_BUILD_OS);
        if (ImGui::CollapsingHeader("Open source licenses"))
        {
            for (auto& license : g_licenses)
            {
                if (ImGui::TreeNode(license.m_library_name.c_str()))
                {
                    ImGui::TextWrapped("%s", license.m_license_text.c_str());
                    ImGui::TreePop();
                }
            }
        }
        if (ImGui::Button("Visit Website"))
        {
            SDL_OpenURL("https://mfeproject.itch.io");
        }
        ImGui::SameLine();
        if (ImGui::Button("Close"))
        {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

bool Application::is_done() const
{
    return m_is_done;
}

}
