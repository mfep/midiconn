#pragma once
#include <filesystem>
#include <string>

#include "SDL2/SDL.h"

#include "ApplicationName.hpp"
#include "Commands.hpp"
#include "ConfigFile.hpp"
#include "MidiEngine.hpp"
#include "NodeEditor.hpp"
#include "NodeFactory.hpp"
#include "PortNameDisplay.hpp"
#include "PresetManager.hpp"
#include "Theme.hpp"
#include "UpdateChecker.hpp"
#include "WelcomeWindow.hpp"

namespace mc
{

class Application final
{
public:
    Application(SDL_Window*                  window,
                SDL_Renderer*                renderer,
                const std::filesystem::path& path_to_preset);
    ~Application();
    void        render();
    void        update_outside_frame();
    void        handle_done(bool& done);
    std::string get_window_title() const;

    void new_preset();
    void open_preset();
    void save_preset();
    void save_preset_as();
    void exit();

private:
    void render_main_menu();
    bool query_save();

    bool            m_is_done{};
    ConfigFile      m_config;
    ThemeControl    m_theme_control;
    midi::Engine    m_midi_engine;
    NodeFactory     m_node_factory;
    Preset          m_preset;
    PresetManager   m_preset_manager;
    bool            m_debug_log_enabled{};
    PortNameDisplay m_port_name_display;
    UpdateChecker   m_update_checker;
    WelcomeWindow   m_welcome_window;
};

} // namespace mc
