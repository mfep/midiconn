#include "Application.hpp"

#include <filesystem>
#include <fstream>

// for some unknown reason, SDL must be included first
#include "IconsForkAwesome.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "portable-file-dialogs.h"
#include "spdlog/spdlog.h"

#include "ErrorHandler.hpp"
#include "Licenses.hpp"
#include "Node.hpp"
#include "PlatformUtils.hpp"
#include "Utils.hpp"
#include "Version.hpp"

namespace mc
{

Application::Application(SDL_Window*                  window,
                         SDL_Renderer*                renderer,
                         ConfigFile&                  config,
                         const std::filesystem::path& path_to_preset)
    : m_config(&config), m_theme_control(*m_config, window),
      m_node_factory(m_theme_control, m_port_name_display),
      m_preset{NodeEditor(m_node_factory, m_port_name_display, m_theme_control)},
      m_preset_manager(m_preset, m_node_factory, *m_config, m_port_name_display, m_theme_control),
      m_port_name_display(m_config->get_show_full_port_names()),
      m_welcome_enabled(m_config->get_show_welcome() && path_to_preset.empty()),
      m_logo_texture(ResourceLoader::load_texture(renderer, "graphics/mc_logo.png"))
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
    ImGuiID        dockspace_id = ImGui::GetID("My Dockspace");
    ImGuiViewport* viewport     = ImGui::GetMainViewport();

    if (ImGui::DockBuilderGetNode(dockspace_id) == nullptr)
    {
        ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
        ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->Size);
        ImGuiID dock_id_right = 0;
        ImGuiID dock_id_left  = 0;
        ImGui::DockBuilderSplitNode(
            dockspace_id, ImGuiDir_Right, 0.33f, &dock_id_right, &dock_id_left);
        ImGui::DockBuilderDockWindow("Inspector", dock_id_right);
        ImGui::DockBuilderDockWindow("Node Editor", dock_id_left);
        ImGui::DockBuilderFinish(dockspace_id);
    }

    ImGui::DockSpaceOverViewport(dockspace_id, viewport, ImGuiDockNodeFlags_PassthruCentralNode);

    wrap_exception([this]() {
        render_welcome_window();
        render_main_menu();
    });

    ImGui::Begin("Node Editor");
    wrap_exception([this]() {
        m_preset.m_node_editor.render();
    });
    ImGui::End();

    ImGui::Begin("Inspector");
    for (auto* selected_node : m_preset.m_node_editor.get_selected_nodes())
    {
        selected_node->render_inspector();
    }
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
           utils::path_to_utf8str(m_preset_manager.get_opened_path().value_or("Untitled")) +
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
        m_preset_manager = PresetManager(
            m_preset, m_node_factory, *m_config, m_port_name_display, m_theme_control);
    }
}

void Application::open_preset()
{
    spdlog::info("Executing open_preset");
    const auto open_path =
        pfd::open_file("Open preset", ".", {"midiconn presets (*.mcpreset)", "*.mcpreset"})
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
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu(ICON_FK_FILE_O " File"))
        {
            if (ImGui::MenuItem(ICON_FK_FILE_O " New preset", "Ctrl+N"))
            {
                new_preset();
            }
            if (ImGui::MenuItem(ICON_FK_FOLDER_OPEN_O " Open preset", "Ctrl+O"))
            {
                open_preset();
            }
            if (ImGui::MenuItem(ICON_FK_FLOPPY_O " Save preset", "Ctrl+S"))
            {
                save_preset();
            }
            if (ImGui::MenuItem(ICON_FK_FLOPPY_O " Save preset as", "Ctrl+Shift+S"))
            {
                save_preset_as();
            }
            ImGui::Separator();
            if (ImGui::MenuItem(ICON_FK_TIMES " Exit", "Alt+F4"))
            {
                exit();
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
            bool show_full_port_names = m_port_name_display.get_show_full_port_names();
            if (ImGui::MenuItem("Show full port names", nullptr, &show_full_port_names))
            {
                m_port_name_display.set_show_full_port_names(show_full_port_names);
                m_config->set_show_port_full_names(show_full_port_names);
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
                SDL_OpenURL(MC_WEBSITE_URL);
            }
            if (ImGui::MenuItem(ICON_FK_MONEY " Donate"))
            {
                SDL_OpenURL(MC_DONATION_URL);
            }
            if (ImGui::MenuItem(ICON_FK_QUESTION " About"))
            {
                m_welcome_enabled = true;
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
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
        ImGui::Image(reinterpret_cast<ImTextureID>(m_logo_texture.m_texture),
                     ImVec2{96 * scale, 96 * scale});
        ImGui::SameLine();
        ImGui::BeginGroup();
        ImGui::Text("Welcome to %s", MIDI_APPLICATION_NAME);
        ImGui::TextUnformatted(g_current_version_str.c_str());
        ImGui::TextUnformatted(MC_COMMIT_SHA);
        ImGui::TextUnformatted(MC_BUILD_OS);
        ImGui::EndGroup();

        const auto latest_version = m_update_checker.get_latest_version();
        std::visit(
            utils::overloads{
                [](const UpdateChecker::StatusNotSupported&) {},
                [](const UpdateChecker::StatusFetching&) {
                    ImGui::Text("Fetching latest %s version number...", MIDI_APPLICATION_NAME);
                },
                [](const UpdateChecker::StatusError& error) {
                    ImGui::Text("Error fetching latest %s version number", MIDI_APPLICATION_NAME);
                    if (!error.message.empty() && ImGui::IsItemHovered())
                        ImGui::SetTooltip("%s", error.message.c_str());
                },
                [](const UpdateChecker::StatusFetched& fetched) {
                    if (fetched.is_latest_version)
                    {
                        ImGui::Text("Congratulations! You are using the latest "
                                    "version of %s",
                                    MIDI_APPLICATION_NAME);
                    }
                    else
                    {
                        ImGui::Text("A new version of %s is available: %s",
                                    MIDI_APPLICATION_NAME,
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
            new_preset();
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("New preset - add all MIDI devices"))
        {
            new_preset(true);
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Browse preset"))
        {
            open_preset();
            ImGui::CloseCurrentPopup();
        }
        const auto last_preset_opt = m_config->get_last_preset_path();
        ImGui::BeginDisabled(!last_preset_opt);
        ImGui::SameLine();
        if (ImGui::Button("Open last preset"))
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

} // namespace mc
