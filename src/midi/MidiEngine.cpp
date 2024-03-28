#include "MidiEngine.hpp"

#include <iomanip>
#include <sstream>

#include "fmt/format.h"
#include "spdlog/spdlog.h"

#include "MidiMessageView.hpp"
#include "MidiProbe.hpp"

namespace fmt
{

template <>
struct formatter<mc::midi::MessageTypeMask>
{
    constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(const mc::midi::MessageTypeMask& mask, FormatContext& ctx) -> decltype(ctx.out())
    {
        return fmt::format_to(ctx.out(),
                              "[sysex: {}, time: {}, active sense: {}]",
                              mask.m_sysex_enabled,
                              mask.m_time_enabled,
                              mask.m_sensing_enabled);
    }
};

} // namespace fmt

namespace mc::midi
{
namespace
{

void check_input_port(unsigned id, const InputInfo& input_info)
{
    const auto valid_port_name = midi::MidiProbe::get_valid_input_port_name(id);
    if (!valid_port_name.has_value())
    {
        throw std::logic_error("Input port does not exist.");
    }
    if (input_info.m_name != valid_port_name.value())
    {
        throw std::logic_error("Input port name does not match expected.");
    }
}

void check_output_port(unsigned id, const OutputInfo& output_info)
{
    const auto valid_port_name = midi::MidiProbe::get_valid_output_port_name(id);
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

void Engine::create(const InputInfo& input_info, InputObserver* observer)
{
    std::lock_guard guard(m_mutex);
    const auto      id = input_info.m_id;
    check_input_port(id, input_info);
    auto[input_node, b] = m_inputs.emplace(id, input_info);
    if (observer)
    {
        input_node->second.add_observer(observer);
    }
    spdlog::info("Instantiated MIDI input '{}'", input_info.m_name);
}

void Engine::create(const OutputInfo& output_info, OutputObserver* observer)
{
    std::lock_guard guard(m_mutex);
    const auto      id = output_info.m_id;
    check_output_port(id, output_info);
    auto [output_node, b] = m_outputs.emplace(id, output_info);
    if (observer)
    {
        output_node->second.add_observer(observer);
    }
    spdlog::info("Instantiated MIDI output '{}'", output_info.m_name);
}

void Engine::remove(const InputInfo& input_info, InputObserver* observer)
{
    std::lock_guard guard(m_mutex);
    const auto      id = input_info.m_id;
    auto connection_it = std::find_if(m_connections.begin(), m_connections.end(), [id](auto& key_value)
    {
        return key_value.first.first == id;
    });
    m_connections.erase(connection_it);
    m_inputs.erase(id);
    spdlog::info("Removed MIDI input '{}'", input_info.m_name);
}

void Engine::remove(const OutputInfo& output_info, OutputObserver* observer)
{
    std::lock_guard guard(m_mutex);
    const auto      id = output_info.m_id;
    auto connection_it = std::find_if(m_connections.begin(), m_connections.end(), [id](auto& key_value)
    {
        return key_value.first.second == id;
    });
    m_connections.erase(connection_it);
    m_outputs.erase(id);
    spdlog::info("Removed MIDI output '{}'", output_info.m_name);
}

void Engine::connect(size_t input_id, size_t output_id, channel_map channels)
{
    std::lock_guard guard(m_mutex);

    const std::pair<size_t,size_t> key(input_id, output_id);
    if (!m_connections.contains(key))
    {
        m_connections[key] = Connection::create(m_inputs.at(input_id), m_outputs.at(output_id));
    }

    std::stringstream ss;
    ss << std::hex;
    for (int val : channels)
    {
        if (val < 0)
        {
            ss << "_";
        }
        else
        {
            ss << val;
        }
    }
    spdlog::info(
        "Connected input {} to output {} with channel mask 0x{}", input_id, output_id, ss.str());
}

void Engine::disconnect(size_t input_id, size_t output_id)
{
    std::lock_guard guard(m_mutex);
    const std::pair<size_t,size_t> key(input_id, output_id);
    Connection::destroy(m_connections[key]);
    m_connections.erase(key);
    spdlog::info("Disconnected input {} from output {}", input_id, output_id);
}

void Engine::enable_message_types(const MessageTypeMask& mask)
{
    std::lock_guard guard(m_mutex);
    spdlog::info("Enabling message types: {}", mask);
    m_message_type_mask = mask;
    for (auto& [id, input] : m_inputs)
    {
        input.enable_message_types(m_message_type_mask);
    }
}

} // namespace mc::midi
