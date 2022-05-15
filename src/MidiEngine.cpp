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
        return format_to(ctx.out(),
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

Engine::MidiInput::MidiInput(const InputInfo& info) : m_info(info)
{
}

void Engine::MidiInput::open()
{
    m_midi_in.setCallback(message_callback, this);
    m_midi_in.setErrorCallback(error_callback, nullptr);
    m_midi_in.openPort(m_info.m_id);
}

void Engine::MidiInput::enable_message_types(const MessageTypeMask& mask)
{
    m_midi_in.ignoreTypes(!mask.m_sysex_enabled, !mask.m_time_enabled, !mask.m_sensing_enabled);
}

void Engine::MidiInput::message_callback(double /*time_stamp*/,
                                         std::vector<unsigned char>* message,
                                         void*                       user_data)
{
    auto* instance = static_cast<MidiInput*>(user_data);
    instance->raise_message_received(instance->m_info.m_id, *message);
}

void Engine::MidiInput::error_callback(RtMidiError::Type  error_code,
                                       const std::string& message,
                                       void* /*user_data*/)
{
    spdlog::error("Error occured in MIDI input: \"{}\"", message);
}

Engine::MidiOutput::MidiOutput(const OutputInfo& info) : m_info(info)
{
    m_midi_out.setErrorCallback(error_callback, nullptr);
    m_midi_out.openPort(info.m_id);
}

void Engine::MidiOutput::send_message(const std::vector<unsigned char>& message_bytes)
{
    m_midi_out.sendMessage(&message_bytes);
    raise_message_sent(m_info.m_id, message_bytes);
}

void Engine::MidiOutput::error_callback(RtMidiError::Type  error_code,
                                        const std::string& message,
                                        void* /*user_data*/)
{
    spdlog::error("Error occured in MIDI output: \"{}\"", message);
}

void Engine::create(const InputInfo& input_info, InputObserver* observer)
{
    std::lock_guard guard(m_mutex);
    const auto      id = input_info.m_id;
    check_input_port(id, input_info);
    if (id < m_inputs.size() && m_inputs[id] != nullptr)
    {
        if (observer != nullptr)
        {
            m_inputs[id]->m_input.add_observer(observer);
        }
        ++m_inputs[id]->m_counter;
        return;
    }
    if (id >= m_inputs.size())
    {
        m_inputs.resize(id + 1);
    }
    auto& input = m_inputs[id] = std::make_unique<InputItem>(InputItem{1, input_info, {}});
    input->m_input.add_observer(this);
    if (observer != nullptr)
    {
        input->m_input.add_observer(observer);
    }
    input->m_input.open();
    spdlog::info("Instantiated MIDI input '{}'", input_info.m_name);
}

void Engine::create(const OutputInfo& output_info, OutputObserver* observer)
{
    std::lock_guard guard(m_mutex);
    const auto      id = output_info.m_id;
    check_output_port(id, output_info);
    if (id < m_outputs.size() && m_outputs[id] != nullptr)
    {
        if (observer != nullptr)
        {
            m_outputs[id]->m_output.add_observer(observer);
        }
        ++m_outputs[id]->m_counter;
        return;
    }
    if (id >= m_outputs.size())
    {
        m_outputs.resize(id + 1);
    }
    auto& output = m_outputs[id] = std::make_unique<OutputItem>(OutputItem{1, output_info});
    if (observer != nullptr)
    {
        output->m_output.add_observer(observer);
    }
    spdlog::info("Instantiated MIDI output '{}'", output_info.m_name);
}

void Engine::remove(const InputInfo& input_info, InputObserver* observer)
{
    std::lock_guard guard(m_mutex);
    const auto      id = input_info.m_id;
    if (id >= m_inputs.size() || m_inputs[id] == nullptr)
    {
        throw std::logic_error("Cannot remove non-existent input");
    }
    if (observer != nullptr)
    {
        m_inputs[id]->m_input.remove_observer(observer);
    }
    auto& counter = m_inputs[id]->m_counter;
    if (--counter == 0)
    {
        m_inputs[id] = nullptr;
        spdlog::info("Removed MIDI input '{}'", input_info.m_name);
    }
}

void Engine::remove(const OutputInfo& output_info, OutputObserver* observer)
{
    std::lock_guard guard(m_mutex);
    const auto      id = output_info.m_id;
    if (id >= m_outputs.size() || m_outputs[id] == nullptr)
    {
        throw std::logic_error("Cannot remove non-existent output");
    }
    auto& output = *m_outputs[id];
    if (observer != nullptr)
    {
        output.m_output.remove_observer(observer);
    }
    if (--output.m_counter > 0)
    {
        return;
    }
    m_outputs[id] = nullptr;
    for (auto& input_ptr : m_inputs)
    {
        if (input_ptr == nullptr)
        {
            continue;
        }
        auto& connected_outputs = input_ptr->m_connections;
        auto  connected_id_iter = connected_outputs.find(id);
        if (connected_id_iter != connected_outputs.end())
        {
            connected_outputs.erase(connected_id_iter);
        }
    }
    spdlog::info("Removed MIDI output '{}'", output_info.m_name);
}

void Engine::connect(size_t input_id, size_t output_id, channel_map channels)
{
    std::lock_guard guard(m_mutex);
    if (input_id >= m_inputs.size() || m_inputs[input_id] == nullptr)
    {
        throw std::logic_error("Cannot connect non-existent input");
    }

    auto& out_list      = m_inputs[input_id]->m_connections;
    out_list[output_id] = channels;

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
    if (input_id >= m_inputs.size() || m_inputs[input_id] == nullptr)
    {
        return;
    }
    auto& out_list = m_inputs[input_id]->m_connections;
    auto  out_itr  = out_list.find(output_id);
    if (out_itr != out_list.cend())
    {
        out_list.erase(out_itr);
    }
    spdlog::info("Disconnected input {} from output {}", input_id, output_id);
}

void Engine::enable_message_types(const InputInfo& input_info, const MessageTypeMask& mask)
{
    // locking can be omitted here as long as this method is not called
    // concurrently with the other methods above
    const auto id = input_info.m_id;
    check_input_port(id, input_info);
    if (id >= m_inputs.size() || m_inputs[id] == nullptr)
    {
        throw std::logic_error("Cannot enable message types on non-existent input");
    }
    spdlog::info("Enabling message types for \"{}\": {}", input_info.m_name, mask);
    m_inputs[id]->m_input.enable_message_types(mask);
}

void Engine::message_received(size_t id, std::vector<unsigned char>& message_bytes)
{
    assert(!message_bytes.empty() && message_bytes.size() <= 3);
    SPDLOG_DEBUG("Message ({} bytes) received from input ID {}", message_bytes.size(), id);
    std::shared_lock lock(m_mutex);
    if (id >= m_inputs.size() || m_inputs[id] == nullptr)
    {
        spdlog::error("Message received from non-existing input");
        return;
    }
    for (auto& [output_id, channels] : m_inputs[id]->m_connections)
    {
        if (output_id >= m_outputs.size() || m_outputs[output_id] == nullptr)
        {
            spdlog::error("Message sent to non-existing output");
            continue;
        }
        auto        message_copy = message_bytes;
        MessageView message(message_copy);
        if (!message.is_system())
        {
            const auto message_channel = message.get_channel();
            const auto target_channel  = channels[message_channel];
            if (target_channel < 0)
            {
                continue;
            }
            message.set_channel(target_channel);
        }
        SPDLOG_DEBUG("Sending message to output ID {}", output_id);
        m_outputs[output_id]->m_output.send_message(message_copy);
    }
}

} // namespace mc::midi
