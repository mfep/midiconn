#include "Application.hpp"

#include <fstream>
#include <filesystem>

// for some unknown reason, SDL must be included first
#include "SDL2/SDL.h"
#include "imgui.h"
#include "nlohmann/json.hpp"
#include "portable-file-dialogs.h"
#include "spdlog/spdlog.h"

#include "Licenses.hpp"
#include "Version.hpp"

namespace mc::display
{

Application::Application(const char* exe_path) :
    m_exe_path(exe_path),
    m_node_editor(m_midi_engine),
    m_preset_manager(m_node_editor, m_midi_engine, exe_path)
{
    spdlog::info("Starting " MIDI_APPLICATION_NAME " version {}", MC_FULL_VERSION);
    auto last_opened_editor = m_preset_manager.try_loading_last_preset();
    if (last_opened_editor.has_value())
    {
        m_node_editor = std::move(last_opened_editor.value());
    }
}

Application::~Application()
{
    spdlog::info(MIDI_APPLICATION_NAME " shutting down");
}

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
        SPDLOG_DEBUG("Application is done");
    }
}

std::string Application::get_window_title() const
{
    auto prefix = m_preset_manager.is_dirty(m_node_editor) ? "* " : "";
    return prefix + m_preset_manager.get_opened_path().value_or("Untitled") + " - " MIDI_APPLICATION_NAME;
}

void Application::handle_shortcuts(const KeyboardShortcutAggregator& shortcuts)
{
    if (shortcuts.is_shortcut_pressed(KeyboardShortcut::CtrlN))
    {
        new_preset_command();
    }
    if (shortcuts.is_shortcut_pressed(KeyboardShortcut::CtrlO))
    {
        open_preset_command();
    }
    if (shortcuts.is_shortcut_pressed(KeyboardShortcut::CtrlS))
    {
        save_preset_command();
    }
    if (shortcuts.is_shortcut_pressed(KeyboardShortcut::CtrlShiftS))
    {
        save_preset_as_command();
    }
}

void Application::new_preset_command()
{
    spdlog::info("Executing new_preset_command");
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

void Application::open_preset_command()
{
    spdlog::info("Executing open_preset_command");
    const auto open_path = pfd::open_file("Open preset", ".", {"JSON files (*.json)", "*.json"}).result();
    if (open_path.size() == 1 && !open_path.front().empty())
    {
        m_node_editor = m_preset_manager.open_preset(open_path.front());
    }
}

void Application::save_preset_command()
{
    spdlog::info("Executing save_preset_command");
    m_preset_manager.save_preset(m_node_editor);
}

void Application::save_preset_as_command()
{
    spdlog::info("Executing save_preset_as_command");
    m_preset_manager.save_preset_as(m_node_editor);
}

void Application::exit_command()
{
    spdlog::info("Executing exit_command");
    m_is_done = true;
}

void Application::render_main_menu()
{
    bool open_about_popup = false;
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("New preset", "Ctrl+N"))
            {
                new_preset_command();
            }
            if (ImGui::MenuItem("Open preset", "Ctrl+O"))
            {
                open_preset_command();
            }
            if (ImGui::MenuItem("Save preset", "Ctrl+S"))
            {
                save_preset_command();
            }
            if (ImGui::MenuItem("Save preset as", "Ctrl+Shift+S"))
            {
                save_preset_as_command();
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Exit", "Alt+F4"))
            {
                exit_command();
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Help"))
        {
            if (ImGui::MenuItem("Enable debug log", nullptr, &m_debug_log_enabled))
            {
                spdlog::set_level(m_debug_log_enabled ? spdlog::level::debug : spdlog::level::info);
                spdlog::info("Debug log enabled: {}", m_debug_log_enabled);
            }
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
        ImGui::TextUnformatted(MC_FULL_VERSION);
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
