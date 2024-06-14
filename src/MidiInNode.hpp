#pragma once

#include "ActivityIndicator.hpp"
#include "Node.hpp"
#include "midi/GraphObserver.hpp"
#include "midi/MessageTypeMask.hpp"
#include "midi/MidiInfo.hpp"

#include <optional>

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
    MidiInNode(std::string_view                 input_name,
               std::shared_ptr<midi::InputNode> midi_input_node,
               const PortNameDisplay&           port_name_display);

    ~MidiInNode();

    void accept_serializer(nlohmann::json& j, const NodeSerializer& serializer) const override;

protected:
    midi::Node* get_midi_node() override;

private:
    void render_internal() override;
    void message_processed(std::span<const unsigned char> message_bytes) override;

    void check_midi_node_connected();
    void set_message_type_mask(midi::MessageTypeMask new_value);

    std::string                      m_input_name;
    std::shared_ptr<midi::InputNode> m_midi_input_node;
    ActivityIndicator                m_midi_activity;
    const PortNameDisplay*           m_port_name_display;
    midi::MessageTypeMask            m_message_type_mask;

    friend class NodeSerializer;
};

} // namespace mc
