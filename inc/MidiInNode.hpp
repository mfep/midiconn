#pragma once
#include "MidiEngine.hpp"
#include "Node.hpp"

namespace mc
{

class MidiInNode final : public Node
{
public:
    MidiInNode(const midi::InputInfo& input_info);

private:
    void render_internal() override;
    void process_internal(size_t size, const_data_itr in_itr, data_itr out_itr) override;

    static void handle_input(double timeStamp, std::vector<unsigned char>* message, void* data);

    midi::InputInfo m_input_info;
    RtMidiIn m_midi_input;
};

}
