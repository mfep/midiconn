#pragma once
#include <chrono>
#include "InputObserver.hpp"
#include "Node.hpp"

namespace mc
{
namespace midi
{
class Engine;
}

class MidiInNode final : public Node, private midi::InputObserver
{
public:
    MidiInNode(const midi::InputInfo& input_info, midi::Engine& midi_engine);
    ~MidiInNode();

    void accept_serializer(nlohmann::json& j, const NodeSerializer& serializer) const override;

private:
    void render_internal() override;
    void message_received(size_t id, std::vector<unsigned char>& message_bytes) override;

    midi::InputInfo m_input_info;
    midi::Engine& m_midi_engine;
    std::chrono::time_point<std::chrono::system_clock> m_last_message_received;

    friend class NodeSerializer;
};

}
