#pragma once
#include <string>

#include "ConfigFile.hpp"
#include "KeyboardShotcutAggregator.hpp"
#include "MidiEngine.hpp"
#include "NodeEditor.hpp"
#include "PresetManager.hpp"
#include "Theme.hpp"

#define MIDI_APPLICATION_NAME       "MIDI Connector"
#define MIDI_APPLICATION_NAME_SNAKE "midi_connector"

namespace mc::display
{

class Application final
{
public:
    Application(const char* exe_path);
    ~Application();
    void        render();
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

    const char*   m_exe_path;
    bool          m_is_done{};
    midi::Engine  m_midi_engine;
    NodeEditor    m_node_editor;
    ConfigFile    m_config;
    PresetManager m_preset_manager;
    ThemeControl  m_theme_control;
    bool          m_debug_log_enabled{};
};

} // namespace mc::display
