#include "MidiEngine.hpp"
#include <algorithm>
#include <iomanip>
#include <sstream>
#include "MidiBuffer.hpp"
#include "spdlog/spdlog.h"

namespace mc::midi
{
namespace
{

template<class RtMidiT, class InfoT>
std::vector<InfoT> get_connections()
{
    RtMidiT midiConn;
    const auto portCount = midiConn.getPortCount();
    std::vector<InfoT> infoList;
    for (unsigned i = 0; i < portCount; i++)
    {
        infoList.push_back({i, midiConn.getPortName(i)});
    }
    return infoList;
}

}   // namespace

std::vector<InputInfo> Probe::get_inputs()
{
    return get_connections<RtMidiIn, InputInfo>();
}

std::vector<OutputInfo> Probe::get_outputs()
{
    return get_connections<RtMidiOut, OutputInfo>();
}

Engine::MidiInput::MidiInput(const InputInfo& info) :
    m_info(info)
{
}

void Engine::MidiInput::open()
{
    m_midiIn.setCallback(message_callback, this);
    m_midiIn.openPort(m_info.m_id);
}

void Engine::MidiInput::message_callback(
    double /*time_stamp*/,
    std::vector<unsigned char> *message,
    void *user_data)
{
    auto* instance = static_cast<MidiInput*>(user_data);
    instance->raise_message_received(instance->m_info.m_id, *message);
}

Engine::MidiOutput::MidiOutput(const OutputInfo& info) :
    m_info(info)
{
    m_midiOut.openPort(info.m_id);
}

void Engine::MidiOutput::send_message(const std::vector<unsigned char>& message_bytes)
{
    m_midiOut.sendMessage(&message_bytes);
}

void Engine::create(const InputInfo& input_info, InputObserver* observer)
{
    std::lock_guard guard(m_mutex);
    const auto id = input_info.m_id;
    if (id < m_inputs.size() && m_inputs[id].has_value())
    {
        ++m_inputs[id].value().m_counter;
        return;
    }
    if (id >= m_inputs.size())
    {
        m_inputs.resize(id + 1);
    }
    auto& input = m_inputs[id].emplace(InputItem{ 1, input_info, {} });
    input.m_input.add_observer(this);
    if (observer != nullptr)
    {
        input.m_input.add_observer(observer);
    }
    input.m_input.open();
    spdlog::info("instantiated MIDI input '{}'", input_info.m_name);
}

void Engine::create(const OutputInfo& output_info)
{
    std::lock_guard guard(m_mutex);
    const auto id = output_info.m_id;
    if (id < m_outputs.size() && m_outputs[id].has_value())
    {
        ++m_outputs[id].value().m_counter;
        return;
    }
    if (id >= m_outputs.size())
    {
        m_outputs.resize(id + 1);
    }
    m_outputs[id].emplace(OutputItem{ 1, output_info });
    spdlog::info("instantiated MIDI output '{}'", output_info.m_name);
}

void Engine::remove(const InputInfo& input_info, InputObserver* observer)
{
    std::lock_guard guard(m_mutex);
    const auto id = input_info.m_id;
    if (id >= m_inputs.size() || !m_inputs[id].has_value())
    {
        throw std::logic_error("Cannot remove non-existent input");
    }
    auto& counter = m_inputs[id].value().m_counter;
    if (observer != nullptr)
    {
        m_inputs[id].value().m_input.remove_observer(observer);
    }
    if (--counter == 0)
    {
        m_inputs[id] = std::nullopt;
        spdlog::info("removed MIDI input '{}'", input_info.m_name);
    }
}

void Engine::remove(const OutputInfo& output_info)
{
    std::lock_guard guard(m_mutex);
    const auto id = output_info.m_id;
    if (id >= m_outputs.size() || !m_outputs[id].has_value())
    {
        throw std::logic_error("Cannot remove non-existent output");
    }
    if (--m_outputs[id].value().m_counter > 0)
    {
        return;
    }
    m_outputs[id] = std::nullopt;
    for (auto& input_opt : m_inputs)
    {
        if (!input_opt.has_value())
        {
            continue;
        }
        auto& connected_outputs = input_opt.value().m_connections;
        auto connected_id_iter = connected_outputs.find(id);
        if (connected_id_iter != connected_outputs.end())
        {
            connected_outputs.erase(connected_id_iter);
        }
    }
    spdlog::info("removed MIDI output '{}'", output_info.m_name);
}

void Engine::connect(size_t input_id, size_t output_id, channel_map channels)
{
    std::lock_guard guard(m_mutex);
    if (input_id >= m_inputs.size() || !m_inputs[input_id].has_value())
    {
        throw std::logic_error("Cannot connect non-existent input");
    }

    auto& out_list = m_inputs[input_id].value().m_connections;
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
    spdlog::info("connected input {} to output {} with channel mask 0x{}",
    input_id, output_id, ss.str());
}

void Engine::disconnect(size_t input_id, size_t output_id)
{
    std::lock_guard guard(m_mutex);
    if (input_id >= m_inputs.size() || !m_inputs[input_id].has_value())
    {
        return;
    }
    auto& out_list = m_inputs[input_id].value().m_connections;
    auto out_itr = out_list.find(output_id);
    if (out_itr != out_list.cend())
    {
        out_list.erase(out_itr);
    }
    spdlog::info("disconnected input {} from output {}", input_id, output_id);
}

void Engine::message_received(size_t id, std::vector<unsigned char>& message_bytes)
{
    assert(message_bytes.size() <= 3); // a message is never longer than 3 bytes
    std::shared_lock lock(m_mutex);
    if (id >= m_inputs.size() || !m_inputs[id].has_value())
    {
        throw std::logic_error("Message received from non-existing input");
    }
    for (auto&[output_id, channels] : m_inputs[id].value().m_connections)
    {
        if (output_id >= m_outputs.size() || !m_outputs[output_id].has_value())
        {
            throw std::logic_error("Message sent to non-existing output");
        }
        MidiBuffer buffer(message_bytes);
        auto message = buffer.begin();
        if (!message.is_system())
        {
            const auto message_channel = message.get_channel();
            const auto target_channel = channels[message_channel];
            if (target_channel < 0)
            {
                return;
            }
            message.set_channel(target_channel);
        }
        m_outputs[output_id].value().m_output.send_message(message_bytes);
    }
}

}   // namespace mt::midi
