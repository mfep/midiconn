#pragma once
#include "Node.hpp"

namespace mc
{

class MidiChannelNode final : public Node
{
private:
    void render_internal() override;
    channel_map transform_channel_map(const channel_map& in_map) override;
};

}
