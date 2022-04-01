#include "Application.hpp"
#include <fstream>
#include <filesystem>
#include "imgui.h"
#include <SDL2/SDL.h>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
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

}   // namespace

namespace mc::display
{

Application::Application(const char* exe_path) :
    m_is_done(false),
    m_midi_engine(std::make_unique<midi::Engine>()),
    m_node_editor(std::make_unique<NodeEditor>(*m_midi_engine)),
    m_exe_path(exe_path)
{
    m_node_editor->to_json(m_last_editor_state);
    try_loading_last_preset();
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
        m_node_editor->render();
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
        if (is_editor_dirty())
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
        try_saving_last_preset_path();
    }
}

std::string Application::get_window_title() const
{
    auto prefix = is_editor_dirty() ? "* " : "";
    return prefix + m_opened_path.value_or("Untitled") + " - " MIDI_APPLICATION_NAME;
}

bool Application::is_editor_dirty() const
{
    nlohmann::json current_editor_state;
    m_node_editor->to_json(current_editor_state);
    return current_editor_state != m_last_editor_state;
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
                if (is_editor_dirty())
                {
                    new_preset = query_save();
                }
                if (new_preset)
                {
                    m_node_editor = std::make_unique<NodeEditor>(*m_midi_engine);
                    m_node_editor->to_json(m_last_editor_state);
                    m_opened_path = std::nullopt;
                }
            }
            if (ImGui::MenuItem("Open preset"))
            {
                const auto open_path = pfd::open_file("Open preset", ".", { "JSON files (*.json)", "*.json" }).result();
                if (open_path.size() == 1 && !open_path.front().empty())
                {
                    open_preset(open_path.front());
                }
            }
            if (ImGui::MenuItem("Save preset"))
            {
                save_preset();
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

void Application::open_preset(const std::string& open_path)
{
    std::ifstream ifs(open_path);
    nlohmann::json j;
    ifs >> j;
    m_node_editor = NodeEditor::from_json(*m_midi_engine, j);
    m_last_editor_state = j;
    m_opened_path = open_path;
}

void Application::save_preset()
{
    auto save_path = pfd::save_file("Save preset", ".", { "JSON files (*.json)", "*.json" }).result();
    if (!save_path.empty())
    {
        if (!ends_with_dot_json(save_path))
        {
            save_path += ".json";
        }
        nlohmann::json j;
        m_node_editor->to_json(j);
        std::ofstream ofs(save_path);
        ofs << j << std::endl;
        m_last_editor_state = j;
        m_opened_path = save_path;
    }
}

bool Application::query_save()
{
    const auto button = pfd::message(MIDI_APPLICATION_NAME, "Do you want to save changes?", pfd::choice::yes_no_cancel).result();
    switch (button)
    {
    case pfd::button::yes:
        save_preset();
        break;
    case pfd::button::no:
    default:
        break;
    case pfd::button::cancel:
        return false;
    }
    return true;
}

void Application::try_loading_last_preset()
{
    std::string previous_preset_path;
    nlohmann::json j;
    try
    {
        std::filesystem::path exe_fs_path(m_exe_path);
        const auto json_path = exe_fs_path.replace_extension("json");
        {
            std::ifstream ifs(json_path);
            ifs >> j;
        }
        j["previous_preset_path"].get_to(previous_preset_path);
    }
    catch(std::exception& ex)
    {
        spdlog::info("Could not load config file. Reason: \"{}\"", ex.what());
        return;
    }
    try
    {
        open_preset(previous_preset_path);
    }
    catch(std::exception& ex)
    {
        spdlog::info("Could not load previous preset at \"{}\". Reason: \"{}\"", previous_preset_path, ex.what());
    }
}

void Application::try_saving_last_preset_path() const
{
    if (!m_opened_path.has_value())
    {
        return;
    }
    std::filesystem::path exe_fs_path(m_exe_path);
    const auto json_path = exe_fs_path.replace_extension("json");
    try
    {
        nlohmann::json j;
        j["previous_preset_path"] = m_opened_path.value();
        std::ofstream ofstream(json_path);
        ofstream << j;
    }
    catch(std::exception& ex)
    {
        spdlog::warn("Could not save previous preset. Reason: \"{}\"", ex.what());
    }
}

}
