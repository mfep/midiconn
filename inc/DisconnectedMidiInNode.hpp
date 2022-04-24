#pragma once
#include "Node.hpp"

namespace mc
{

class DisconnectedMidiInNode final : public Node
{
public:
    DisconnectedMidiInNode(const std::string& input_name);
    void accept_serializer(nlohmann::json& j, const NodeSerializer& serializer) const override;

private:
    void render_internal() override;

    std::string m_input_name;
    friend class NodeSerializer;
};

}
