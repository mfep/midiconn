#include "Application.hpp"
#include <fstream>
#include <filesystem>
#include "imgui.h"
#include <SDL2/SDL.h>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include "portable-file-dialogs.h"
#include "Licenses.hpp"
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

}   // namespace

namespace mc::display
{

Application::Application(const char* exe_path) :
    m_exe_path(exe_path),
    m_node_editor(m_midi_engine),
    m_preset_manager(m_node_editor, m_midi_engine, exe_path)
{
    auto last_opened_editor = m_preset_manager.try_loading_last_preset();
    if (last_opened_editor.has_value())
    {
        m_node_editor = std::move(last_opened_editor.value());
    }
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

    try
    {
        render_main_menu();
        m_node_editor.render();
    }
    catch(std::exception& ex)
    {
        spdlog::error(ex.what());
        pfd::message("Error", ex.what(), pfd::choice::ok, pfd::icon::error);
    }

    ImGui::End();
#ifndef NDEBUG
    ImGui::ShowDemoWindow();
#endif
}

void Application::handle_done(bool& done)
{
    if (done || m_is_done)
    {
        if (m_preset_manager.is_dirty(m_node_editor))
        {
            done = m_is_done = query_save();
        }
        else
        {
            done = m_is_done = true;
        }
    }
    if (done)
    {
        m_preset_manager.try_saving_last_preset_path();
    }
}

std::string Application::get_window_title() const
{
    auto prefix = m_preset_manager.is_dirty(m_node_editor) ? "* " : "";
    return prefix + m_preset_manager.get_opened_path().value_or("Untitled") + " - " MIDI_APPLICATION_NAME;
}

void Application::render_main_menu()
{
    bool open_about_popup = false;
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("New preset"))
            {
                bool new_preset = true;
                if (m_preset_manager.is_dirty(m_node_editor))
                {
                    new_preset = query_save();
                }
                if (new_preset)
                {
                    m_node_editor = NodeEditor(m_midi_engine);
                    m_preset_manager = PresetManager(m_node_editor, m_midi_engine, m_exe_path);
                }
            }
            if (ImGui::MenuItem("Open preset"))
            {
                const auto open_path = pfd::open_file("Open preset", ".", { "JSON files (*.json)", "*.json" }).result();
                if (open_path.size() == 1 && !open_path.front().empty())
                {
                    m_node_editor = m_preset_manager.open_preset(open_path.front());
                }
            }
            if (ImGui::MenuItem("Save preset"))
            {
                m_preset_manager.save_preset(m_node_editor);
            }
            if (ImGui::MenuItem("Save preset as"))
            {
                m_preset_manager.save_preset_as(m_node_editor);
            }
            ImGui::Separator();
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

bool Application::query_save()
{
    const auto button = pfd::message(MIDI_APPLICATION_NAME, "Do you want to save changes?", pfd::choice::yes_no_cancel).result();
    switch (button)
    {
    case pfd::button::yes:
        m_preset_manager.save_preset(m_node_editor);
        break;
    case pfd::button::no:
    default:
        break;
    case pfd::button::cancel:
        return false;
    }
    return true;
}

}
