#pragma once
#include <cstdlib>
#include <map>
#include <memory>
#include <shared_mutex>
#include <vector>

#include "RtMidi.h"

#include "InputObserver.hpp"
#include "MidiInfo.hpp"
#include "OutputObserver.hpp"

namespace mc::midi
{

struct MessageTypeMask
{
    bool m_sysex_enabled{true};
    bool m_time_enabled{true};
    bool m_sensing_enabled{true};
};

class Engine final : private InputObserver
{
private:
    class MidiInput final : public InputObservable
    {
    public:
        MidiInput(const InputInfo& info);
        void        open();
        void        enable_message_types(const MessageTypeMask& mask);
        static void message_callback(double                      time_stamp,
                                     std::vector<unsigned char>* message,
                                     void*                       user_data);
        static void error_callback(RtMidiError::Type  error_code,
                                   const std::string& message,
                                   void*              user_data);

    private:
        InputInfo m_info;
        RtMidiIn  m_midi_in;
    };

    class MidiOutput final : public OutputObservable
    {
    public:
        MidiOutput(const OutputInfo& info);
        void        send_message(const std::vector<unsigned char>& message_bytes);
        static void error_callback(RtMidiError::Type  error_code,
                                   const std::string& message,
                                   void*              user_data);

    private:
        OutputInfo m_info;
        RtMidiOut  m_midi_out;
    };

public:
    void create(const InputInfo& input_info, InputObserver* observer = nullptr);
    void create(const OutputInfo& output_info, OutputObserver* observer = nullptr);
    void remove(const InputInfo& input_info, InputObserver* observer = nullptr);
    void remove(const OutputInfo& output_info, OutputObserver* observer = nullptr);
    void connect(size_t input_id, size_t output_id, channel_map channels);
    void disconnect(size_t input_id, size_t output_id);
    void enable_message_types(const MessageTypeMask& mask);

    const MessageTypeMask& get_message_types() const { return m_message_type_mask; }

private:
    void message_received(size_t id, std::vector<unsigned char>& message_bytes) override;

    struct InputItem
    {
        size_t                        m_counter;
        MidiInput                     m_input;
        std::map<size_t, channel_map> m_connections;
    };

    struct OutputItem
    {
        size_t     m_counter;
        MidiOutput m_output;
    };

    std::vector<std::unique_ptr<InputItem>>  m_inputs;
    std::vector<std::unique_ptr<OutputItem>> m_outputs;
    std::shared_mutex                        m_mutex;
    MessageTypeMask                          m_message_type_mask;
};

} // namespace mc::midi
