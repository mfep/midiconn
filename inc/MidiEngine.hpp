#pragma once
#include <optional>
#include <shared_mutex>
#include <string>
#include <vector>
#include "RtMidi.h"

namespace mc::midi
{

struct InputInfo final
{
    size_t m_id{};
    std::string m_name;
};

struct OutputInfo final
{
    size_t m_id{};
    std::string m_name;
};

class Probe final
{
public:
    Probe() = delete;

    static std::vector<InputInfo> get_inputs();
    static std::vector<OutputInfo> get_outputs();
};

class InputObserver
{
public:
    virtual ~InputObserver() = default;
    virtual void message_received(size_t id, const std::vector<unsigned char>& message_bytes) = 0;
};

class InputObservable
{
public:
    virtual ~InputObservable() = default;
    void add_observer(InputObserver* observer);
    void remove_observer(InputObserver* observer);

protected:
    void raise_message_received(size_t id, const std::vector<unsigned char>& message_bytes) const;

private:
    std::vector<InputObserver*> m_observers;
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

    class MidiOutput final
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
    void connect(size_t input_id, size_t output_id);
    void disconnect(size_t input_id, size_t output_id);

private:
    void message_received(size_t id, const std::vector<unsigned char>& message_bytes) override;

    std::vector<
        std::optional<
            std::tuple<
                MidiInput,           // input
                std::vector<size_t>, // connected outputs
                size_t               // counter
            >
        >
    > m_inputs;
    std::vector<
        std::optional<
            std::tuple<
                MidiOutput,          // output
                size_t               // counter
            >
        >
    > m_outputs;
    std::shared_mutex m_mutex;
};

}
