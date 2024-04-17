#pragma once
#include "imgui.h"
#include "imnodes.h"

#include "Node.hpp"
#include "NodeSerializer.hpp"

namespace mc
{

template <class Derived>
class DisconnectedNode : public Node
{
public:
    void accept_serializer(nlohmann::json& j, const NodeSerializer& serializer) const override
    {
        serializer.serialize_node(j, *static_cast<const Derived*>(this));
    }

    void push_style() const override
    {
        ImNodes::PushColorStyle(ImNodesCol_TitleBar, IM_COL32(127, 127, 127, 255));
        ImNodes::PushColorStyle(ImNodesCol_TitleBarHovered, IM_COL32(117, 117, 117, 255));
        ImNodes::PushColorStyle(ImNodesCol_TitleBarSelected, IM_COL32(117, 117, 117, 255));
    }

    void pop_style() const override
    {
        ImNodes::PopColorStyle();
        ImNodes::PopColorStyle();
        ImNodes::PopColorStyle();
    }

protected:
    midi::Node* get_midi_node() override { return nullptr; }
};

} // namespace mc
