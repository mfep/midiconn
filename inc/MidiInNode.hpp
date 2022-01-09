#pragma once
#include "Node.hpp"

namespace mc
{
namespace midi
{
class Engine;
}

class MidiInNode final : public Node
{
public:
    MidiInNode(const midi::InputInfo& input_info, midi::Engine& midi_engine);
    ~MidiInNode();

private:
    void render_internal() override;

    midi::InputInfo m_input_info;
    midi::Engine& m_midi_engine;
};

}
