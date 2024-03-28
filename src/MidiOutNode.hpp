#pragma once
#include <chrono>

#include "Node.hpp"
#include "midi/OutputObserver.hpp"

namespace mc
{
namespace midi
{
class Engine;
}

class PortNameDisplay;

class MidiOutNode final : public Node, private midi::OutputObserver
{
public:
    MidiOutNode(const midi::OutputInfo& output_info,
                midi::Engine&           midi_engine,
                const PortNameDisplay&  port_name_display);
    ~MidiOutNode();

    void accept_serializer(nlohmann::json& j, const NodeSerializer& serializer) const override;
    const midi::OutputInfo& get_info() const { return m_output_info; }

private:
    void render_internal() override;
    void update_outputs_with_sources() override;
    void message_sent(size_t id, const std::vector<unsigned char>& message_bytes) override;

    midi::OutputInfo                                   m_output_info;
    midi::Engine*                                      m_midi_engine;
    Node::midi_sources                                 m_previous_sources;
    std::chrono::time_point<std::chrono::system_clock> m_last_message_sent;
    const PortNameDisplay*                             m_port_name_display;

    friend class NodeSerializer;
};

} // namespace mc
