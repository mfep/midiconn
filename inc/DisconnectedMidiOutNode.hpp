#pragma once
#include "Node.hpp"

namespace mc
{

class DisconnectedMidiOutNode final : public Node
{
public:
    DisconnectedMidiOutNode(const std::string& output_name);
    void accept_serializer(nlohmann::json& j, const NodeSerializer& serializer) const override;

private:
    void render_internal() override;

    std::string m_output_name;
    friend class NodeSerializer;
};

}
