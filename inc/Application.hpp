#pragma once
#include <string>

#include "SDL2/SDL.h"

#include "ConfigFile.hpp"
#include "KeyboardShotcutAggregator.hpp"
#include "MidiEngine.hpp"
#include "NodeEditor.hpp"
#include "NodeFactory.hpp"
#include "PresetManager.hpp"
#include "Theme.hpp"

#define MIDI_APPLICATION_NAME       "MIDI Connector"
#define MIDI_APPLICATION_NAME_SNAKE "midi_connector"

namespace mc::display
{

class Application final
{
public:
    Application(const std::string& exe_path, SDL_Window* window);
    ~Application();
    void        render();
    void        update_outside_frame();
    void        handle_done(bool& done);
    std::string get_window_title() const;
    void        handle_shortcuts(const KeyboardShortcutAggregator& shortcuts);

private:
    void new_preset_command();
    void open_preset_command();
    void save_preset_command();
    void save_preset_as_command();
    void exit_command();

    void render_main_menu();
    bool query_save();

    std::string   m_exe_path;
    bool          m_is_done{};
    ConfigFile    m_config;
    ThemeControl  m_theme_control;
    midi::Engine  m_midi_engine;
    NodeFactory   m_node_factory;
    Preset        m_preset;
    PresetManager m_preset_manager;
    bool          m_debug_log_enabled{};
};

} // namespace mc::display
