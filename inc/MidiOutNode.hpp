#pragma once
#include "Node.hpp"
#include "MidiEngine.hpp"

namespace mc
{

class MidiOutNode final : public Node
{
public:
    MidiOutNode(const midi::OutputInfo& output_info);

private:
    void render_internal() override;
    void process_internal(size_t size, const_data_itr in_itr, data_itr out_itr) override;

    midi::OutputInfo m_output_info;
    RtMidiOut m_midi_output;
};

}
