#include "MidiProbe.hpp"

#include <algorithm>
#include <random>
#include <regex>
#include <sstream>

#include "RtMidi.h"

#include "ApplicationName.hpp"

namespace mc
{

using namespace midi;

namespace
{

template <class RtMidiT, class InfoT>
std::vector<InfoT> get_connections()
{
    RtMidiT            midi_conn;
    const auto         port_count = midi_conn.getPortCount();
    std::vector<InfoT> info_list;
    for (unsigned i = 0; i < port_count; i++)
    {
        std::string port_name = midi_conn.getPortName(i);
        // Omit self
        if (port_name.find(MidiProbe::get_midi_client_name()) == std::string::npos)
        {
            info_list.push_back({i, std::move(port_name)});
        }
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

std::string generate_random_client_name()
{
    std::stringstream sstream;
    sstream << MIDI_APPLICATION_NAME_SNAKE << '_' << std::hex;
    std::default_random_engine    prng(std::random_device{}());
    std::uniform_int_distribution dist(0, 16);

    for (std::size_t i = 0; i < 4; ++i)
    {
        sstream << dist(prng);
    }
    return sstream.str();
}

} // namespace

std::string MidiProbe::get_midi_client_name()
{
    static std::string name = generate_random_client_name();
    return name;
}

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
    return RtMidiIn{}.getPortName(id);
}

std::optional<std::string> MidiProbe::get_valid_output_port_name(unsigned id)
{
    if (RtMidiOut{}.getPortCount() <= id)
    {
        return std::nullopt;
    }
    return RtMidiOut{}.getPortName(id);
}

} // namespace mc
