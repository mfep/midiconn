#include "MidiProbe.hpp"

#include <algorithm>

#include "RtMidi.h"

namespace mc
{

using namespace midi;

namespace
{

template<class RtMidiT, class InfoT>
std::vector<InfoT> get_connections()
{
    RtMidiT midi_conn;
    const auto port_count = midi_conn.getPortCount();
    std::vector<InfoT> info_list;
    for (unsigned i = 0; i < port_count; i++)
    {
        info_list.push_back({i, midi_conn.getPortName(i)});
    }
    return info_list;
}

template<class Info>
std::optional<Info> get_valid_info(const std::string& name, const std::vector<Info>& infos)
{
    auto found_it = std::find_if(infos.begin(), infos.end(), [&](const auto& info) { return info.m_name == name; });
    if (found_it != infos.end())
    {
        return *found_it;
    }
    return std::nullopt;
}

}   // namespace

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

}