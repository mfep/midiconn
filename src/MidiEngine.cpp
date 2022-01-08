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

void Engine::create(const InputInfo& input_info)
{
    std::lock_guard guard(m_mutex);
    const auto id = input_info.m_id;
    if (id < m_inputs.size() && m_inputs[id].has_value())
    {
        ++std::get<2>(m_inputs[id].value());
        return;
    }
    if (id >= m_inputs.size())
    {
        m_inputs.resize(id + 1);
    }
    auto& input = m_inputs[id].emplace(std::make_tuple<MidiInput, std::vector<size_t>>(input_info, {}, 1));
    std::get<0>(input).add_observer(this);
    std::get<0>(input).open();
}

void Engine::create(const OutputInfo& output_info)
{
    std::lock_guard guard(m_mutex);
    const auto id = output_info.m_id;
    if (id < m_outputs.size() && m_outputs[id].has_value())
    {
        ++std::get<1>(m_outputs[id].value());
        return;
    }
    if (id >= m_outputs.size())
    {
        m_outputs.resize(id + 1);
    }
    m_outputs[id].emplace(std::make_tuple(output_info, 1));
}

void Engine::remove(const InputInfo& input_info)
{
    std::lock_guard guard(m_mutex);
    const auto id = input_info.m_id;
    if (id >= m_inputs.size() || !m_inputs[id].has_value())
    {
        throw std::logic_error("Cannot remove non-existent input");
    }
    auto& counter = std::get<2>(m_inputs[id].value());
    if (--counter == 0)
    {
        m_inputs[id] = std::nullopt;
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
    if (--std::get<1>(m_outputs[id].value()) > 0)
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
        auto& [input, connected_output_ids, count] = input_opt.value();
        auto connected_id_iter = std::find(
            connected_output_ids.begin(),
            connected_output_ids.end(),
            id);
        if (connected_id_iter != connected_output_ids.end())
        {
            connected_output_ids.erase(connected_id_iter);
        }
    }
}

void Engine::connect(size_t input_id, size_t output_id)
{
    std::lock_guard guard(m_mutex);

    if (input_id >= m_inputs.size() || !m_inputs[input_id].has_value())
    {
        throw std::logic_error("Cannot connect non-existent input");
    }

    auto& out_list = std::get<1>(m_inputs[input_id].value());
    auto out_itr = std::find(out_list.cbegin(), out_list.cend(), output_id);
    if (out_itr == out_list.cend())
    {
        out_list.push_back(output_id);
        std::cout << "Connected " << input_id << " to " << output_id << std::endl;
    }
}

void Engine::disconnect(size_t input_id, size_t output_id)
{
    std::lock_guard guard(m_mutex);
    if (input_id >= m_inputs.size() || !m_inputs[input_id].has_value())
    {
        return;
    }
    auto& out_list = std::get<1>(m_inputs[input_id].value());
    auto out_itr = std::find(out_list.cbegin(), out_list.cend(), output_id);
    if (out_itr == out_list.cend())
    {
        return;
    }
    out_list.erase(out_itr);

    std::cout << "Disconnected " << input_id << " from " << output_id << std::endl;
}

void Engine::message_received(size_t id, const std::vector<unsigned char>& message_bytes)
{
    std::shared_lock lock(m_mutex);
    if (id >= m_inputs.size() || !m_inputs[id].has_value())
    {
        throw std::logic_error("Message received from non-existing input");
    }
    for (size_t output_id : std::get<1>(m_inputs[id].value()))
    {
        if (output_id >= m_outputs.size() || !m_outputs[output_id].has_value())
        {
            throw std::logic_error("Message sent to non-existing output");
        }
        std::get<0>(m_outputs[output_id].value()).send_message(message_bytes);
    }
}

}   // namespace mt::midi
