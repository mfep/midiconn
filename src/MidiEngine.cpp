#include "MidiEngine.hpp"

#include <algorithm>

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

void InputObservable::add_observer(InputObserver* observer)
{
    auto found_observer = std::find(m_observers.cbegin(), m_observers.cend(), observer);
    if (found_observer == m_observers.cend())
    {
        m_observers.push_back(observer);
    }
}

void InputObservable::remove_observer(InputObserver* observer)
{
    auto found_observer = std::find(m_observers.cbegin(), m_observers.cend(), observer);
    if (found_observer != m_observers.cend())
    {
        m_observers.erase(found_observer);
    }
}

void InputObservable::raise_message_received(
    size_t id,
    const std::vector<unsigned char>& message_bytes) const
{
    for (auto* observer : m_observers)
    {
        observer->message_received(id, message_bytes);
    }
}

Engine::MidiInput::MidiInput(const InputInfo& info) :
    m_info(info)
{
    m_midiIn.setCallback(message_callback, this);
    m_midiIn.openPort(info.m_id);
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

void Engine::create(const InputInfo& input_info)
{
    const auto id = input_info.m_id;
    if (id < m_inputs.size() && m_inputs[id].has_value())
    {
        // input already exists, do nothing
        return;
    }
    if (id >= m_inputs.size())
    {
        m_inputs.resize(id + 1);
    }
    m_inputs[id].emplace(input_info);
}

void Engine::create(const OutputInfo& output_info)
{
    const auto id = output_info.m_id;
    if (id < m_outputs.size() && m_outputs[id].has_value())
    {
        return;
    }
    if (id >= m_outputs.size())
    {
        m_outputs.resize(id + 1);
    }
    m_outputs[id].emplace(output_info);
}

void Engine::remove(const InputInfo& input_info)
{
    const auto id = input_info.m_id;
    if (id >= m_inputs.size())
    {
        return;
    }
    m_inputs[id] = std::nullopt;
}

void Engine::remove(const OutputInfo& output_info)
{
    const auto id = output_info.m_id;
    if (id >= m_outputs.size())
    {
        return;
    }
    m_outputs[id] = std::nullopt;
}

void Engine::connect(const OutputInfo& output_info, const InputInfo& input_info)
{
    const auto out_id = output_info.m_id;
    const auto in_id = input_info.m_id;

    if (out_id >= m_connections.size())
    {
        m_connections.resize(out_id + 1);
    }
    if (!m_connections[out_id].has_value())
    {
        m_connections[out_id].emplace();
    }
    auto& in_list = m_connections[out_id].value();
    auto in_itr = std::find(in_list.cbegin(), in_list.cend(), in_id);
    if (in_itr == in_list.cend())
    {
        in_list.push_back(in_id);
    }
}

void Engine::disconnect(const OutputInfo& output_info, const InputInfo& input_info)
{
    const auto out_id = output_info.m_id;
    const auto in_id = input_info.m_id;
    if (out_id >= m_connections.size())
    {
        return;
    }
    if (!m_connections[out_id].has_value())
    {
        return;
    }
    auto& in_list = m_connections[out_id].value();
    auto in_itr = std::find(in_list.cbegin(), in_list.cend(), in_id);
    if (in_itr == in_list.cend())
    {
        return;
    }
    in_list.erase(in_itr);
}

void Engine::message_received(size_t id, const std::vector<unsigned char>& message_bytes)
{
    if (!m_connections[id].has_value())
    {
        return;
    }
    for (size_t output_id : m_connections[id].value())
    {
        m_outputs[output_id].value().send_message(message_bytes);
    }
}

}   // namespace mt::midi
