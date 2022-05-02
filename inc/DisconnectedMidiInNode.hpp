#pragma once
#include "DisconnectedNode.hpp"

namespace mc
{

class DisconnectedMidiInNode final : public DisconnectedNode<DisconnectedMidiInNode>
{
public:
    DisconnectedMidiInNode(const std::string& input_name);
    const std::string& get_name() const { return m_input_name; }

private:
    void render_internal() override;

    std::string m_input_name;
    friend class NodeSerializer;
};

}
