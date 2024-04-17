#pragma once

#include "ActivityIndicator.hpp"
#include "Node.hpp"
#include "midi/GraphObserver.hpp"

namespace mc
{
namespace midi
{
class OutputNode;
}

class PortNameDisplay;

class MidiOutNode final : public Node, private midi::GraphObserver
{
public:
    MidiOutNode(const midi::OutputInfo&           output_info,
                std::shared_ptr<midi::OutputNode> midi_output_node,
                const PortNameDisplay&            port_name_display);

    ~MidiOutNode();

    void accept_serializer(nlohmann::json& j, const NodeSerializer& serializer) const override;
    const midi::OutputInfo& get_info() const { return m_output_info; }

protected:
    midi::Node* get_midi_node() override;

private:
    void render_internal() override;
    void message_received(std::span<const unsigned char> message_bytes) override;

    midi::OutputInfo                  m_output_info;
    std::shared_ptr<midi::OutputNode> m_midi_output_node;
    ActivityIndicator                 m_midi_activity;
    const PortNameDisplay*            m_port_name_display;

    friend class NodeSerializer;
};

} // namespace mc
