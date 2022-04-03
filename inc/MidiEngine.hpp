#pragma once
#include <cstdlib>
#include <map>
#include <optional>
#include <shared_mutex>
#include <vector>

#include "RtMidi.h"

#include "InputObserver.hpp"
#include "MidiInfo.hpp"
#include "OutputObserver.hpp"

namespace mc::midi
{

class Probe final
{
public:
    Probe() = delete;

    static std::vector<InputInfo> get_inputs();
    static std::vector<OutputInfo> get_outputs();
};

class Engine final : private InputObserver
{
private:
    class MidiInput final : public InputObservable
    {
    public:
        MidiInput(const InputInfo& info);
        void open();
        static void message_callback(double time_stamp, std::vector<unsigned char> *message, void *user_data);

    private:
        InputInfo m_info;
        RtMidiIn m_midiIn;
    };

    class MidiOutput final : public OutputObservable
    {
    public:
        MidiOutput(const OutputInfo& info);
        void send_message(const std::vector<unsigned char>& message_bytes);

    private:
        OutputInfo m_info;
        RtMidiOut m_midiOut;
    };

public:
    void create(const InputInfo& input_info, InputObserver* observer = nullptr);
    void create(const OutputInfo& output_info, OutputObserver* observer = nullptr);
    void remove(const InputInfo& input_info, InputObserver* observer = nullptr);
    void remove(const OutputInfo& output_info, OutputObserver* observer = nullptr);
    void connect(size_t input_id, size_t output_id, channel_map channels);
    void disconnect(size_t input_id, size_t output_id);

private:
    void message_received(size_t id, std::vector<unsigned char>& message_bytes) override;

    struct InputItem
    {
        size_t m_counter;
        MidiInput m_input;
        std::map<size_t, channel_map> m_connections;
    };

    struct OutputItem
    {
        size_t m_counter;
        MidiOutput m_output;
    };

    std::vector<std::optional<InputItem>> m_inputs;
    std::vector<std::optional<OutputItem>> m_outputs;
    std::shared_mutex m_mutex;
};

}
