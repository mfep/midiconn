#pragma once
#include "DisconnectedNode.hpp"

namespace mc
{

class PortNameDisplay;

class DisconnectedMidiInNode final : public DisconnectedNode<DisconnectedMidiInNode>
{
public:
    DisconnectedMidiInNode(const std::string& input_name, const PortNameDisplay& port_name_display);
    const std::string& get_name() const { return m_input_name; }

private:
    void render_internal() override;

    std::string            m_input_name;
    const PortNameDisplay* m_port_name_display;
    friend class NodeSerializer;
};

} // namespace mc
