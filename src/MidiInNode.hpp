#pragma once

#include "ActivityIndicator.hpp"
#include "Node.hpp"
#include "midi/GraphObserver.hpp"
#include "midi/MidiInfo.hpp"

namespace mc
{
namespace midi
{
class InputNode;
}

class PortNameDisplay;

class MidiInNode final : public Node, private midi::GraphObserver
{
public:
    MidiInNode(const midi::InputInfo&           input_info,
               std::shared_ptr<midi::InputNode> midi_input_node,
               const PortNameDisplay&           port_name_display);

    ~MidiInNode();

    void accept_serializer(nlohmann::json& j, const NodeSerializer& serializer) const override;
    const midi::InputInfo& get_info() const { return m_input_info; }

protected:
    midi::Node* get_midi_node() override;

private:
    void render_internal() override;
    void message_processed(std::span<const unsigned char> message_bytes) override;

    midi::InputInfo                  m_input_info;
    std::shared_ptr<midi::InputNode> m_midi_input_node;
    ActivityIndicator                m_midi_activity;
    const PortNameDisplay*           m_port_name_display;

    friend class NodeSerializer;
};

} // namespace mc
