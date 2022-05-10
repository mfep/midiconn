#pragma once
#include <string>

#include "MidiEngine.hpp"
#include "NodeEditor.hpp"
#include "PresetManager.hpp"

#define MIDI_APPLICATION_NAME "MIDI Connector"
#define MIDI_APPLICATION_NAME_SNAKE "midi_connector"

namespace mc
{
namespace midi
{
class Engine;
}
namespace display
{
class NodeEditor;

class Application final
{
public:
    Application(const char* exe_path);
    ~Application();
    void render();
    void handle_done(bool& done);
    std::string get_window_title() const;

private:
    void new_preset_command();
    void open_preset_command();
    void save_preset_command();
    void save_preset_as_command();
    void exit_command();

    void render_main_menu();
    bool query_save();

    const char* m_exe_path;
    bool m_is_done{};
    midi::Engine m_midi_engine;
    NodeEditor m_node_editor;
    PresetManager m_preset_manager;
    bool m_debug_log_enabled{};
};

}
}
