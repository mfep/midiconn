#pragma once

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
class ScaleProvider;

class MidiInNode final : public Node
{
public:
    MidiInNode(std::string_view                 input_name,
               std::shared_ptr<midi::InputNode> midi_input_node,
               const PortNameDisplay&           port_name_display,
               const ScaleProvider&             scale_provider);

    void accept_serializer(nlohmann::json& j, const NodeSerializer& serializer) const override;
    void render_inspector() override;

private:
    void render_internal() override;

    void check_midi_node_connected();
    void set_message_type_mask(midi::MessageTypeMask new_value);

    std::string                      m_input_name;
    std::shared_ptr<midi::InputNode> m_midi_input_node;
    const PortNameDisplay*           m_port_name_display;
    midi::MessageTypeMask            m_message_type_mask;

    friend class NodeSerializer;
};

} // namespace mc
