#include "OutputNode.hpp"

#include "MidiProbe.hpp"

#include <spdlog/spdlog.h>

namespace mc::midi
{
namespace
{

void check_output_port(unsigned id, const OutputInfo& output_info)
{
    const auto valid_port_name = MidiProbe::get_valid_output_port_name(id);
    if (!valid_port_name.has_value())
    {
        throw std::logic_error("Output port does not exist.");
    }
    if (output_info.m_name != valid_port_name.value())
    {
        throw std::logic_error("Output port name does not match expected.");
    }
}

} // namespace
} // namespace mc::midi

mc::midi::OutputNode::OutputNode(const OutputInfo& info) : m_info(info)
{
    check_output_port(info.m_id, info);
    m_midi_out.setErrorCallback(error_callback, nullptr);
    m_midi_out.openPort(info.m_id);
    spdlog::info("Opened MIDI output \"{}\" ({})", info.m_name, info.m_id);
}

mc::midi::OutputNode::~OutputNode()
{
    spdlog::info("Closed MIDI output \"{}\" ({})", m_info.m_name, m_info.m_id);
}

bool mc::midi::OutputNode::process(data_span data)
{
    m_midi_out.sendMessage(data.data(), data.size());
    return true;
}

std::string_view mc::midi::OutputNode::name()
{
    return m_info.m_name;
}

void mc::midi::OutputNode::error_callback(RtMidiError::Type  error_code,
                                          const std::string& message,
                                          void* /*user_data*/)
{
    spdlog::error("Error {} occured in MIDI output: \"{}\"", static_cast<int>(error_code), message);
}
