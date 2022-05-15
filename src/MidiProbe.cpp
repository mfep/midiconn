#include "MidiProbe.hpp"

#include <algorithm>
#include <sstream>

#include "RtMidi.h"

namespace mc
{

using namespace midi;

namespace
{

// ToDo check on linux
std::string truncate_port_name(const std::string& port_name)
{
    const auto last_space_idx = port_name.find_last_of(' ');
    if (last_space_idx == std::string::npos)
    {
        return port_name;
    }
    for (size_t idx = last_space_idx + 1; idx < port_name.size(); idx++)
    {
        if (!std::isdigit(port_name[idx]))
        {
            return port_name;
        }
    }
    return port_name.substr(0, last_space_idx);
}

template <class RtMidiT, class InfoT>
std::vector<InfoT> get_connections()
{
    RtMidiT            midi_conn;
    const auto         port_count = midi_conn.getPortCount();
    std::vector<InfoT> info_list;
    for (unsigned i = 0; i < port_count; i++)
    {
        info_list.push_back({i, truncate_port_name(midi_conn.getPortName(i))});
    }
    return info_list;
}

template <class Info>
std::optional<Info> get_valid_info(const std::string& name, const std::vector<Info>& infos)
{
    auto found_it = std::find_if(infos.begin(), infos.end(), [&](const auto& info) {
        return info.m_name == name;
    });
    if (found_it != infos.end())
    {
        return *found_it;
    }
    return std::nullopt;
}

} // namespace

std::vector<InputInfo> MidiProbe::get_inputs()
{
    return get_connections<RtMidiIn, InputInfo>();
}

std::vector<OutputInfo> MidiProbe::get_outputs()
{
    return get_connections<RtMidiOut, OutputInfo>();
}

std::optional<InputInfo> MidiProbe::get_valid_input(const std::string& input_name)
{
    return get_valid_info(input_name, get_inputs());
}

std::optional<OutputInfo> MidiProbe::get_valid_output(const std::string& output_name)
{
    return get_valid_info(output_name, get_outputs());
}

std::optional<std::string> MidiProbe::get_valid_input_port_name(unsigned id)
{
    if (RtMidiIn{}.getPortCount() <= id)
    {
        return std::nullopt;
    }
    return truncate_port_name(RtMidiIn{}.getPortName(id));
}

std::optional<std::string> MidiProbe::get_valid_output_port_name(unsigned id)
{
    if (RtMidiOut{}.getPortCount() <= id)
    {
        return std::nullopt;
    }
    return truncate_port_name(RtMidiOut{}.getPortName(id));
}

} // namespace mc