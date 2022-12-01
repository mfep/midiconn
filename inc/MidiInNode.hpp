#pragma once
#include <chrono>

#include "InputObserver.hpp"
#include "MidiEngine.hpp"
#include "MidiInfo.hpp"
#include "Node.hpp"

namespace mc
{
namespace midi
{
class Engine;
}

class PortNameDisplay;

class MidiInNode final : public Node, private midi::InputObserver
{
public:
    MidiInNode(const midi::InputInfo& input_info,
               midi::Engine&          midi_engine,
               const PortNameDisplay& port_name_display);
    ~MidiInNode();

    void accept_serializer(nlohmann::json& j, const NodeSerializer& serializer) const override;
    const midi::InputInfo& get_info() const { return m_input_info; }

private:
    void render_internal() override;
    void message_received(size_t id, std::vector<unsigned char>& message_bytes) override;

    midi::InputInfo                                    m_input_info;
    midi::Engine*                                      m_midi_engine;
    std::chrono::time_point<std::chrono::system_clock> m_last_message_received;
    const PortNameDisplay*                             m_port_name_display;

    friend class NodeSerializer;
};

} // namespace mc
