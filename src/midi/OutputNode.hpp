#pragma once
#include "MidiGraph.hpp"
#include "MidiInfo.hpp"
#include "OutputObserver.hpp"

#include "RtMidi.h"

namespace mc::midi
{

class OutputNode final : public Node, public OutputObservable
{
public:
    explicit OutputNode(const OutputInfo& info);

    void process(data_span data) override;

private:
    static void error_callback(RtMidiError::Type  error_code,
                               const std::string& message,
                               void*              user_data);

    OutputInfo m_info;
    RtMidiOut  m_midi_out;
};

} // namespace mc::midi
