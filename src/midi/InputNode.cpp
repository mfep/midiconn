#include "InputNode.hpp"

#include "MidiProbe.hpp"

#include <spdlog/spdlog.h>

namespace mc::midi
{
namespace
{

void check_input_port(unsigned id, const InputInfo& input_info)
{
    const auto valid_port_name = MidiProbe::get_valid_input_port_name(id);
    if (!valid_port_name.has_value())
    {
        throw std::logic_error("Input port does not exist.");
    }
    if (input_info.m_name != valid_port_name.value())
    {
        throw std::logic_error("Input port name does not match expected.");
    }
}

} // namespace
} // namespace mc::midi

mc::midi::InputNode::InputNode(const InputInfo& info)
    : m_info(info), m_midi_in(RtMidi::UNSPECIFIED, MidiProbe::get_midi_client_name())
{
    check_input_port(info.m_id, info);
    m_midi_in.setCallback(message_callback, this);
    m_midi_in.setErrorCallback(error_callback, nullptr);
    m_midi_in.openPort(m_info.m_id);
    spdlog::info("Opened MIDI input \"{}\" ({})", info.m_name, info.m_id);
}

mc::midi::InputNode::~InputNode()
{
    spdlog::info("Closed MIDI input \"{}\" ({})", m_info.m_name, m_info.m_id);
}

void mc::midi::InputNode::enable_message_types(const MessageTypeMask& mask)
{
    m_midi_in.ignoreTypes(!mask.m_sysex_enabled, !mask.m_time_enabled, !mask.m_sensing_enabled);
}

std::string_view mc::midi::InputNode::name()
{
    return m_info.m_name;
}

void mc::midi::InputNode::message_callback(double /*time_stamp*/,
                                           std::vector<unsigned char>* message,
                                           void*                       user_data)
{
    auto* instance = static_cast<InputNode*>(user_data);
    instance->raise_message_processed(*message);
    instance->broadcast_data(*message);
}

void mc::midi::InputNode::error_callback(RtMidiError::Type  error_code,
                                         const std::string& message,
                                         void* /*user_data*/)
{
    spdlog::error("Error {} occured in MIDI input: \"{}\"", static_cast<int>(error_code), message);
}
