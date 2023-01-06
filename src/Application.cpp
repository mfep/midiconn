#include "Application.hpp"

#include <filesystem>
#include <fstream>

// for some unknown reason, SDL must be included first
#include "IconsForkAwesome.h"
#include "imgui.h"
#include "portable-file-dialogs.h"
#include "spdlog/spdlog.h"

#include "ErrorHandler.hpp"
#include "Licenses.hpp"
#include "PlatformUtils.hpp"
#include "Version.hpp"

namespace mc::display
{

Application::Application(SDL_Window* window, const std::filesystem::path& path_to_preset)
    : m_theme_control(m_config, window),
      m_node_factory(m_midi_engine, m_theme_control, m_port_name_display),
      m_preset{NodeEditor(m_node_factory, m_port_name_display)},
      m_preset_manager(m_preset, m_node_factory, m_config, m_port_name_display),
      m_port_name_display(m_config.get_show_full_port_names())
{
    spdlog::info("Starting " MIDI_APPLICATION_NAME " version {}", MC_FULL_VERSION);
    std::optional<Preset> opened_preset;
    if (path_to_preset.empty())
    {
        opened_preset = m_preset_manager.try_loading_last_preset();
    }
    else
    {
        opened_preset = m_preset_manager.open_preset(path_to_preset);
    }
    if (opened_preset.has_value())
    {
        m_preset = std::move(opened_preset.value());
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
    ImGui::Begin("Another Window",
                 nullptr,
                 ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoSavedSettings |
                     ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                     ImGuiWindowFlags_MenuBar);

    wrap_exception([this]() {
        render_main_menu();
        m_preset.m_node_editor.render();
    });

    ImGui::End();
#ifndef NDEBUG
    ImGui::ShowDemoWindow();
#endif
}

void Application::update_outside_frame()
{
    m_theme_control.update_scale_if_needed();
}

void Application::handle_done(bool& done)
{
    if (done || m_is_done)
    {
        if (m_preset_manager.is_dirty(m_preset))
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
    auto prefix = m_preset_manager.is_dirty(m_preset) ? "* " : "";
    return prefix + m_preset_manager.get_opened_path().value_or("Untitled").string() +
           " - " MIDI_APPLICATION_NAME;
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
    if (m_preset_manager.is_dirty(m_preset))
    {
        new_preset = query_save();
    }
    if (new_preset)
    {
        m_preset         = {NodeEditor(m_node_factory, m_port_name_display)};
        m_preset_manager = PresetManager(m_preset, m_node_factory, m_config, m_port_name_display);
    }
}

void Application::open_preset_command()
{
    spdlog::info("Executing open_preset_command");
    const auto open_path =
        pfd::open_file("Open preset", ".", {"midiconn presets (*.mcpreset)", "*.mcpreset"})
            .result();
    if (open_path.size() == 1 && !open_path.front().empty())
    {
        m_preset = m_preset_manager.open_preset(open_path.front());
    }
}

void Application::save_preset_command()
{
    spdlog::info("Executing save_preset_command");
    m_preset_manager.save_preset(m_preset);
}

void Application::save_preset_as_command()
{
    spdlog::info("Executing save_preset_as_command");
    m_preset_manager.save_preset_as(m_preset);
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
        if (ImGui::BeginMenu(ICON_FK_FILE_O " File"))
        {
            if (ImGui::MenuItem(ICON_FK_FILE_O "  New preset", "Ctrl+N"))
            {
                new_preset_command();
            }
            if (ImGui::MenuItem(ICON_FK_FOLDER_OPEN_O " Open preset", "Ctrl+O"))
            {
                open_preset_command();
            }
            if (ImGui::MenuItem(ICON_FK_FLOPPY_O "  Save preset", "Ctrl+S"))
            {
                save_preset_command();
            }
            if (ImGui::MenuItem(ICON_FK_FLOPPY_O "  Save preset as", "Ctrl+Shift+S"))
            {
                save_preset_as_command();
            }
            ImGui::Separator();
            if (ImGui::BeginMenu("MIDI message filter"))
            {
                bool changed = ImGui::MenuItem(
                    "SysEx", nullptr, &m_preset.m_message_type_mask.m_sysex_enabled);
                changed = ImGui::MenuItem(
                              "Clock", nullptr, &m_preset.m_message_type_mask.m_time_enabled) ||
                          changed;
                changed = ImGui::MenuItem("Active Sensing",
                                          nullptr,
                                          &m_preset.m_message_type_mask.m_sensing_enabled) ||
                          changed;
                if (changed)
                {
                    m_midi_engine.enable_message_types(m_preset.m_message_type_mask);
                }
                ImGui::EndMenu();
            }
            ImGui::Separator();
            if (ImGui::MenuItem(" " ICON_FK_TIMES "  Exit", "Alt+F4"))
            {
                exit_command();
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu(ICON_FK_COG " Settings"))
        {
            if (ImGui::BeginMenu(ICON_FK_PAINT_BRUSH " Theme"))
            {
                const auto current_theme = m_theme_control.get_theme();
                if (ImGui::MenuItem(
                        ICON_FK_MOON_O " Dark theme", nullptr, current_theme == Theme::Dark))
                {
                    m_theme_control.set_theme(Theme::Dark);
                }
                if (ImGui::MenuItem(
                        ICON_FK_SUN_O " Light theme", nullptr, current_theme == Theme::Light))
                {
                    m_theme_control.set_theme(Theme::Light);
                }
                if (ImGui::MenuItem(
                        ICON_FK_STAR_O " Classic theme", nullptr, current_theme == Theme::Classic))
                {
                    m_theme_control.set_theme(Theme::Classic);
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu(ICON_FK_EYE " Interface Scale"))
            {
                const auto current_scale = m_theme_control.get_scale();
                for (size_t i = 0; i < interface_scale_labels.size(); ++i)
                {
                    if (ImGui::MenuItem(interface_scale_labels[i].data(),
                                        nullptr,
                                        static_cast<size_t>(current_scale) == i))
                    {
                        m_theme_control.set_scale(static_cast<InterfaceScale>(i));
                    }
                }
                ImGui::EndMenu();
            }
            bool show_full_port_names = m_port_name_display.get_show_full_port_names();
            if (ImGui::MenuItem("Show full port names", nullptr, &show_full_port_names))
            {
                m_port_name_display.set_show_full_port_names(show_full_port_names);
                m_config.set_show_port_full_names(show_full_port_names);
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu(ICON_FK_QUESTION " Help"))
        {
            if (ImGui::MenuItem(ICON_FK_PENCIL " Enable debug log", nullptr, &m_debug_log_enabled))
            {
                spdlog::set_level(m_debug_log_enabled ? spdlog::level::debug : spdlog::level::info);
                spdlog::info("Debug log enabled: {}", m_debug_log_enabled);
            }
            if (ImGui::MenuItem(ICON_FK_PENCIL_SQUARE " Open logfile"))
            {
                platform::open_logfile_external();
            }
            if (ImGui::MenuItem(ICON_FK_GLOBE " Visit website"))
            {
                SDL_OpenURL("https://mfeproject.itch.io/midiconn");
            }
            if (ImGui::MenuItem(ICON_FK_QUESTION "  About"))
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
    bool dummy{true};
    if (ImGui::BeginPopupModal(
            "About " MIDI_APPLICATION_NAME, &dummy, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::TextUnformatted(MC_FULL_VERSION);
        ImGui::TextUnformatted(MC_COMMIT_SHA);
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
        ImGui::EndPopup();
    }
}

bool Application::query_save()
{
    const auto button = pfd::message(MIDI_APPLICATION_NAME,
                                     "Do you want to save changes?",
                                     pfd::choice::yes_no_cancel)
                            .result();
    switch (button)
    {
    case pfd::button::yes:
        m_preset_manager.save_preset(m_preset);
        break;
    case pfd::button::no:
    default:
        break;
    case pfd::button::cancel:
        return false;
    }
    return true;
}

} // namespace mc::display
