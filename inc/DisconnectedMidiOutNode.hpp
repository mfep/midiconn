#pragma once
#include "DisconnectedNode.hpp"

namespace mc
{

class DisconnectedMidiOutNode final : public DisconnectedNode<DisconnectedMidiOutNode>
{
public:
    DisconnectedMidiOutNode(const std::string& output_name);

private:
    void render_internal() override;

    std::string m_output_name;
    friend class NodeSerializer;
};

}
