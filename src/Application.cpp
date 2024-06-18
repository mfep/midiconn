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
#include "Utils.hpp"
#include "Version.hpp"

#include "libintl.h"

namespace mc
{

Application::Application(SDL_Window*                  window,
                         SDL_Renderer*                renderer,
                         const std::filesystem::path& path_to_preset)
    : m_theme_control(m_config, window), m_node_factory(m_theme_control, m_port_name_display),
      m_preset{NodeEditor(m_node_factory, m_port_name_display, m_theme_control)},
      m_preset_manager(m_preset, m_node_factory, m_config, m_port_name_display, m_theme_control),
      m_port_name_display(m_config.get_show_full_port_names()),
      m_welcome_enabled(m_config.get_show_welcome() && path_to_preset.empty()),
      m_logo_texture(ResourceLoader::load_texture(renderer, "graphics/mc_logo.png")),
      m_locale(m_config)
{
    spdlog::info("Starting " MIDI_APPLICATION_NAME " version {}", g_current_version);

    if (!path_to_preset.empty())
    {
        wrap_exception([&] {
            m_preset = m_preset_manager.open_preset(path_to_preset);
        });
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
        render_welcome_window();
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
    return prefix +
           utils::path_to_utf8str(
               // Translators: The name of the default preset
               m_preset_manager.get_opened_path().value_or(gettext("Untitled"))) +
           " - " MIDI_APPLICATION_NAME;
}

void Application::new_preset(bool create_nodes)
{
    spdlog::info("Executing new_preset");
    bool new_preset = true;
    if (m_preset_manager.is_dirty(m_preset))
    {
        new_preset = query_save();
    }
    if (new_preset)
    {
        m_preset = {NodeEditor(m_node_factory, m_port_name_display, m_theme_control, create_nodes)};
        m_preset_manager =
            PresetManager(m_preset, m_node_factory, m_config, m_port_name_display, m_theme_control);
    }
}

void Application::open_preset()
{
    spdlog::info("Executing open_preset");
    const auto open_path =
        // Translators: the title of the open preset file browser dialog
        pfd::open_file(gettext("Open preset"), ".", {"midiconn presets (*.mcpreset)", "*.mcpreset"})
            .result();
    if (open_path.size() == 1 && !open_path.front().empty())
    {
        m_preset = m_preset_manager.open_preset(std::filesystem::u8path(open_path.front()));
    }
}

void Application::open_last_preset()
{
    spdlog::info("Executing open_last_preset");
    wrap_exception([&] {
        if (auto preset = m_preset_manager.try_loading_last_preset(); preset.has_value())
        {
            m_preset = std::move(*preset);
        }
    });
}

void Application::save_preset()
{
    spdlog::info("Executing save_preset");
    m_preset_manager.save_preset(m_preset);
}

void Application::save_preset_as()
{
    spdlog::info("Executing save_preset_as");
    m_preset_manager.save_preset_as(m_preset);
}

void Application::exit()
{
    spdlog::info("Executing exit");
    m_is_done = true;
}

void Application::render_main_menu()
{
    if (ImGui::BeginMenuBar())
    {
        // Translators: File menu label
        static auto begin_menu_label = fmt::format("{} {}", ICON_FK_FILE_O, gettext("File"));
        if (ImGui::BeginMenu(begin_menu_label.c_str()))
        {
            static auto new_preset_label =
                // Translators: Menu entry to initialize an empty preset
                fmt::format("{} {}", ICON_FK_FILE_O, gettext("New preset"));
            if (ImGui::MenuItem(new_preset_label.c_str(), "Ctrl+N"))
            {
                new_preset();
            }
            static auto open_preset_label =
                // Translators: Menu entry to open a preset file
                fmt::format("{} {}", ICON_FK_FOLDER_OPEN_O, gettext("Open preset"));
            if (ImGui::MenuItem(open_preset_label.c_str(), "Ctrl+O"))
            {
                open_preset();
            }
            static auto save_preset_label =
                // Translators: Menu entry to save the current preset to file where it was already
                // saved
                fmt::format("{} {}", ICON_FK_FLOPPY_O, gettext("Save preset"));
            if (ImGui::MenuItem(save_preset_label.c_str(), "Ctrl+S"))
            {
                save_preset();
            }
            static auto save_preset_as_label =
                // Translators: Menu entry to save the current preset to file (query location)
                fmt::format("{} {}", ICON_FK_FLOPPY_O, gettext("Save preset as"));
            if (ImGui::MenuItem(save_preset_as_label.c_str(), "Ctrl+Shift+S"))
            {
                save_preset_as();
            }
            ImGui::Separator();
            static auto exit_label =
                // Translators: Menu entry to exit the program
                fmt::format("{} {}", ICON_FK_TIMES, gettext("Exit"));
            if (ImGui::MenuItem(exit_label.c_str(), "Alt+F4"))
            {
                exit();
            }
            ImGui::EndMenu();
        }
        static auto settings_label =
            // Translators: Settings menu label
            fmt::format("{} {}", ICON_FK_COG, gettext("Settings"));
        if (ImGui::BeginMenu(settings_label.c_str()))
        {
            static auto theme_label =
                // Translators: Theme menu label
                fmt::format("{} {}", ICON_FK_PAINT_BRUSH, gettext("Theme"));
            if (ImGui::BeginMenu(theme_label.c_str()))
            {
                const auto  current_theme = m_theme_control.get_theme();
                static auto dark_theme_label =
                    // Translators: Menu entry for selecting the dark theme
                    fmt::format("{} {}", ICON_FK_MOON_O, gettext("Dark theme"));
                if (ImGui::MenuItem(
                        dark_theme_label.c_str(), nullptr, current_theme == Theme::Dark))
                {
                    m_theme_control.set_theme(Theme::Dark);
                }
                static auto light_theme_label =
                    // Translators: Menu entry for selecting the light theme
                    fmt::format("{} {}", ICON_FK_SUN_O, gettext("Light theme"));
                if (ImGui::MenuItem(
                        light_theme_label.c_str(), nullptr, current_theme == Theme::Light))
                {
                    m_theme_control.set_theme(Theme::Light);
                }
                static auto classic_theme_label =
                    // Translators: Menu entry for selecting the classic theme
                    fmt::format("{} {}", ICON_FK_STAR_O, gettext("Classic theme"));
                if (ImGui::MenuItem(
                        classic_theme_label.c_str(), nullptr, current_theme == Theme::Classic))
                {
                    m_theme_control.set_theme(Theme::Classic);
                }
                ImGui::EndMenu();
            }
            static auto interface_scale_label =
                // Translators: Menu entry for selecting the interface scale
                fmt::format("{} {}", ICON_FK_EYE, gettext("Interface Scale"));
            if (ImGui::BeginMenu(interface_scale_label.c_str()))
            {
                const auto current_scale          = m_theme_control.get_scale();
                const auto interface_scale_labels = get_interface_scale_labels();
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
// This kind of locale selection is broken on Windows
#ifndef _WIN32
            static auto language_label =
                // Translators: Menu entry for selecting the display language
                fmt::format("{} {}", ICON_FK_LANGUAGE, gettext("Language"));
            if (ImGui::BeginMenu(language_label.c_str()))
            {
                const auto& current_locale = m_config.get_locale();
                if (ImGui::MenuItem(gettext("Reset to system language")))
                {
                    m_locale.set_locale("");
                }
                const auto supported_locales = Locale::get_supported_locales();
                for (const auto& locale : supported_locales)
                {
                    const bool selected = locale.locale_code == current_locale;
                    if (ImGui::MenuItem(locale.display_name.c_str(), nullptr, selected))
                    {
                        m_locale.set_locale(locale.locale_code);
                    }
                }
                ImGui::EndMenu();
            }
#endif
            bool show_full_port_names = m_port_name_display.get_show_full_port_names();
            // Translators: Menu entry that controls whether the full or abbreviated MIDI port names
            // are displayed
            if (ImGui::MenuItem(gettext("Show full port names"), nullptr, &show_full_port_names))
            {
                m_port_name_display.set_show_full_port_names(show_full_port_names);
                m_config.set_show_port_full_names(show_full_port_names);
            }
            ImGui::EndMenu();
        }
        static auto help_label =
            // Translators: Menu entry for the help submenu
            fmt::format("{} {}", ICON_FK_QUESTION, gettext("Help"));
        if (ImGui::BeginMenu(help_label.c_str()))
        {
            static auto debug_log_label =
                // Translators: Menu entry to enable and disable debug logging
                fmt::format("{} {}", ICON_FK_PENCIL, gettext("Enable debug log"));
            if (ImGui::MenuItem(debug_log_label.c_str(), nullptr, &m_debug_log_enabled))
            {
                spdlog::set_level(m_debug_log_enabled ? spdlog::level::debug : spdlog::level::info);
                spdlog::info("Debug log enabled: {}", m_debug_log_enabled);
            }
            static auto open_logfile_label =
                // Translators: Menu entry to open the log file with the default application
                fmt::format("{} {}", ICON_FK_PENCIL_SQUARE, gettext("Open logfile"));
            if (ImGui::MenuItem(open_logfile_label.c_str()))
            {
                platform::open_logfile_external();
            }
            static auto visit_website_label =
                // Translators: Menu entry to open the project's website with the browser
                fmt::format("{} {}", ICON_FK_GLOBE, gettext("Visit website"));
            if (ImGui::MenuItem(visit_website_label.c_str()))
            {
                SDL_OpenURL(MC_WEBSITE_URL);
            }
            static auto about_label =
                // Translators: Menu entry to open the about window
                fmt::format("{} {}", ICON_FK_QUESTION, gettext("About"));
            if (ImGui::MenuItem(about_label.c_str()))
            {
                m_welcome_enabled = true;
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
}

void Application::render_welcome_window()
{
    if (m_welcome_enabled)
    {
        ImGui::OpenPopup("Welcome");
        m_welcome_enabled = false;
    }
    const ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    const float scale = m_theme_control.get_scale_value();
    ImGui::SetNextWindowSizeConstraints({600 * scale, 0}, {600 * scale, 400 * scale});
    if (ImGui::BeginPopup("Welcome", ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Image(m_logo_texture.m_texture, ImVec2{96 * scale, 96 * scale});
        ImGui::SameLine();
        ImGui::BeginGroup();
        // Translators: Welcome message in the welcome window
        ImGui::Text(gettext("Welcome to %s"), MIDI_APPLICATION_NAME);
        ImGui::TextUnformatted(g_current_version_str.c_str());
        ImGui::TextUnformatted(MC_COMMIT_SHA);
        ImGui::TextUnformatted(MC_BUILD_OS);
        ImGui::EndGroup();

        const auto latest_version = m_update_checker.get_latest_version();
        std::visit(utils::overloads{
                       [](const UpdateChecker::StatusNotSupported&) {},
                       [](const UpdateChecker::StatusFetching&) {
                           // Translators: Message in the welcome window while downloading the
                           // latest version number
                           ImGui::Text(gettext("Fetching latest %s version number..."),
                                       MIDI_APPLICATION_NAME);
                       },
                       [](const UpdateChecker::StatusError& error) {
                           // Translators: Message in the welcome window, when an error occurred
                           // while fetching the version number from the web
                           ImGui::Text(gettext("Error fetching latest %s version number"),
                                       MIDI_APPLICATION_NAME);
                           if (!error.message.empty() && ImGui::IsItemHovered())
                               ImGui::SetTooltip("%s", error.message.c_str());
                       },
                       [](const UpdateChecker::StatusFetched& fetched) {
                           if (fetched.is_latest_version)
                           {
                               // Translators: Message in the welcome window, when the downloaded
                               // latest version is the same as the current application's version
                               ImGui::Text(gettext("Congratulations! You are using the latest "
                                                   "version of %s"),
                                           MIDI_APPLICATION_NAME);
                           }
                           else
                           {
                               // Translators: Message in the welcome window, when the downloaded
                               // latest version is newer than the current application's version
                               ImGui::Text(gettext("A new version of %s is available: %s"),
                                           MIDI_APPLICATION_NAME,
                                           fetched.latest_version_name.c_str());
                               ImGui::SameLine();
                               if (ImGui::Button(gettext("Visit website")))
                               {
                                   SDL_OpenURL(MC_WEBSITE_URL);
                               }
                           }
                       },
                   },
                   latest_version);

        bool show_on_startup = m_config.get_show_welcome();
        // Translators: Checkbox on the welcome window
        if (ImGui::Checkbox(gettext("Show this window on application startup"), &show_on_startup))
        {
            m_config.set_show_welcome(show_on_startup);
        }
        // Translators: Button on the welcome window to create a new empty preset
        if (ImGui::Button(gettext("New preset - empty")))
        {
            new_preset();
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        // Translators: Button on the welcome window to create a new preset filled with all
        // available devices
        if (ImGui::Button(gettext("New preset - add all MIDI devices")))
        {
            new_preset(true);
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        // Translators: Button on the welcome window to browse the filesystem for a preset to load
        if (ImGui::Button(gettext("Browse preset")))
        {
            open_preset();
            ImGui::CloseCurrentPopup();
        }
        const auto last_preset_opt = m_config.get_last_preset_path();
        ImGui::BeginDisabled(!last_preset_opt);
        ImGui::SameLine();
        // Translators: Button on the welcome window to open the last opened preset
        if (ImGui::Button(gettext("Open last preset")))
        {
            open_last_preset();
            ImGui::CloseCurrentPopup();
        }
        if (last_preset_opt && ImGui::IsItemHovered())
        {
            const auto preset_str = utils::path_to_utf8str(*last_preset_opt);
            ImGui::SetTooltip("%s", preset_str.c_str());
        }
        ImGui::EndDisabled();
        // Translators: Button on the welcome window to show the 3rd party library licenses
        if (ImGui::CollapsingHeader(gettext("Open source licenses")))
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
    const auto button =
        pfd::message(
            MIDI_APPLICATION_NAME,
            // Translators: The message on the popup window when attempting to close the program
            gettext("Do you want to save changes?"),
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

} // namespace mc
