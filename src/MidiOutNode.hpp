#pragma once

#include "Node.hpp"
#include "midi/GraphObserver.hpp"

namespace mc
{
namespace midi
{
class OutputNode;
}

class PortNameDisplay;
class ScaleProvider;

class MidiOutNode final : public Node
{
public:
    MidiOutNode(std::string_view                  output_name,
                std::shared_ptr<midi::OutputNode> midi_output_node,
                const PortNameDisplay&            port_name_display,
                const ScaleProvider&              scale_provider);

    void accept_serializer(nlohmann::json& j, const NodeSerializer& serializer) const override;
    void render_inspector() override;

private:
    void render_internal() override;
    void check_midi_node_connected();

    std::string                       m_output_name;
    std::shared_ptr<midi::OutputNode> m_midi_output_node;
    const PortNameDisplay*            m_port_name_display;

    friend class NodeSerializer;
};

} // namespace mc
