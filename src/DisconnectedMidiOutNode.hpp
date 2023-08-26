#pragma once
#include "DisconnectedNode.hpp"

namespace mc
{

class PortNameDisplay;

class DisconnectedMidiOutNode final : public DisconnectedNode<DisconnectedMidiOutNode>
{
public:
    DisconnectedMidiOutNode(const std::string&     output_name,
                            const PortNameDisplay& port_name_display);
    const std::string& get_name() const { return m_output_name; }

private:
    void render_internal() override;

    std::string            m_output_name;
    const PortNameDisplay* m_port_name_display;
    friend class NodeSerializer;
};

} // namespace mc
