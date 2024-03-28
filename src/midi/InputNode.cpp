#include "InputNode.hpp"

#include "MidiProbe.hpp"

#include <spdlog/spdlog.h>

mc::midi::InputNode::InputNode(const InputInfo& info)
    : m_info(info), m_midi_in(RtMidi::UNSPECIFIED, MidiProbe::get_midi_client_name())
{
    m_midi_in.setCallback(message_callback, this);
    m_midi_in.setErrorCallback(error_callback, nullptr);
    m_midi_in.openPort(m_info.m_id);
}

void mc::midi::InputNode::enable_message_types(const MessageTypeMask& mask)
{
    m_midi_in.ignoreTypes(!mask.m_sysex_enabled, !mask.m_time_enabled, !mask.m_sensing_enabled);
}

void mc::midi::InputNode::message_callback(double                      time_stamp,
                                           std::vector<unsigned char>* message,
                                           void*                       user_data)
{
    auto* instance = static_cast<InputNode*>(user_data);
    instance->broadcast_data(*message);
    instance->raise_message_received(instance->m_info.m_id, *message);
}

void mc::midi::InputNode::error_callback(RtMidiError::Type  error_code,
                                         const std::string& message,
                                         void*              user_data)
{
    spdlog::error("Error {} occured in MIDI input: \"{}\"", static_cast<int>(error_code), message);
}
