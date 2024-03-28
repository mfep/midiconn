#pragma once
#include "InputObserver.hpp"
#include "MidiEngine.hpp"
#include "MidiGraph.hpp"
#include "MidiInfo.hpp"

#include "RtMidi.h"

namespace mc::midi
{

class InputNode final : public Node, public InputObservable
{
public:
    explicit InputNode(const InputInfo& info);

    void enable_message_types(const MessageTypeMask& mask);

private:
    static void message_callback(double                      time_stamp,
                                 std::vector<unsigned char>* message,
                                 void*                       user_data);

    static void error_callback(RtMidiError::Type  error_code,
                               const std::string& message,
                               void*              user_data);

    InputInfo m_info;
    RtMidiIn  m_midi_in;
};

} // namespace mc::midi
