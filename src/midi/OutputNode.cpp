#include "OutputNode.hpp"

#include <spdlog/spdlog.h>

mc::midi::OutputNode::OutputNode(const OutputInfo& info)
{
    m_midi_out.setErrorCallback(error_callback, nullptr);
    m_midi_out.openPort(info.m_id);
}

void mc::midi::OutputNode::process(data_span data)
{
    m_midi_out.sendMessage(data.data(), data.size());
}

void mc::midi::OutputNode::error_callback(RtMidiError::Type  error_code,
                                          const std::string& message,
                                          void*              user_data)
{
    spdlog::error("Error {} occured in MIDI output: \"{}\"", static_cast<int>(error_code), message);
}
