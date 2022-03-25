#pragma once
#include <memory>
#include <nlohmann/json.hpp>

#define MIDI_APPLICATION_NAME "MIDI Connector"

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
    Application();
    ~Application();
    void render();
    void handle_done(bool& done);
    std::string get_window_title() const;

private:
    bool is_editor_dirty() const;
    void render_main_menu();
    void open_preset();
    void save_preset();
    bool quit();

    bool m_is_done;
    std::unique_ptr<midi::Engine> m_midi_engine;
    std::unique_ptr<NodeEditor> m_node_editor;
    nlohmann::json m_last_editor_state;
    std::string m_opened_filename{ "Untitled" };
};

}
}
