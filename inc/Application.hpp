#pragma once
#include <memory>

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
    bool is_done() const;

private:
    void render_main_menu();

    bool m_is_done;
    std::unique_ptr<midi::Engine> m_midi_engine;
    std::unique_ptr<NodeEditor> m_node_editor;
};

}
}
