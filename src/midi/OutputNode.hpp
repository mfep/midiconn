#pragma once
#include "MidiGraph.hpp"
#include "MidiInfo.hpp"

#include "RtMidi.h"

#include <string_view>

namespace mc::midi
{

class OutputNode final : public Node
{
public:
    explicit OutputNode(const OutputInfo& info);
    ~OutputNode();

    bool process(data_span data) override;

protected:
    std::string_view name() override;

private:
    static void error_callback(RtMidiError::Type  error_code,
                               const std::string& message,
                               void*              user_data);

    OutputInfo m_info;
    RtMidiOut  m_midi_out;
};

} // namespace mc::midi
