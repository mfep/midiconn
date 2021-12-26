#pragma once
#include <optional>
#include <string>
#include <vector>
#include "RtMidi.h"

namespace mc::midi
{

struct InputInfo
{
    size_t m_id{};
    std::string m_name;
};

struct OutputInfo
{
    size_t m_id{};
    std::string m_name;
};

class Probe
{
public:
    static std::vector<InputInfo> get_inputs();
    static std::vector<OutputInfo> get_outputs();
};

class Engine
{
private:
    class MidiInput
    {
    public:
        MidiInput(const InputInfo& info);
        static void message_callback(double time_stamp, std::vector<unsigned char> *message, void *user_data);

    private:
        InputInfo m_info;
        RtMidiIn m_midiIn;
    };

    class MidiOutput
    {
    public:
        MidiOutput(const OutputInfo& info);
        void send_message(const std::vector<unsigned char>& message_bytes);

    private:
        OutputInfo m_info;
        RtMidiOut m_midiOut;
    };

public:
    void create(const InputInfo& input_info);
    void create(const OutputInfo& output_info);
    void remove(const InputInfo& input_info);
    void remove(const OutputInfo& output_info);
    void connect(const OutputInfo& output_info, const InputInfo& input_info);
    void disconnect(const OutputInfo& output_info, const InputInfo& input_info);

private:
    std::vector<std::optional<MidiInput>> m_inputs;
    std::vector<std::optional<MidiOutput>> m_outputs;
    std::vector<std::optional<std::vector<size_t>>> m_connections;
};

}
