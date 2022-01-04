#pragma once
#include "Node.hpp"
#include "MidiEngine.hpp"

namespace mc
{

class MidiOutNode final : public Node
{
public:
    MidiOutNode(const midi::OutputInfo& output_info, midi::Engine& midi_engine);
    ~MidiOutNode();

private:
    void render_internal() override;
    void update_outputs() override;

    midi::OutputInfo m_output_info;
    midi::Engine& m_midi_engine;
    Node::midi_sources m_previous_sources;
};

}
