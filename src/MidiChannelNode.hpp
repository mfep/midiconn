#pragma once

#include <functional>

#include "Node.hpp"
#include "midi/ChannelMapNode.hpp"

namespace mc
{

class ScaleProvider;

class MidiChannelNode final : public Node
{
public:
    MidiChannelNode(const ScaleProvider&                  scale_provider,
                    std::shared_ptr<midi::ChannelMapNode> midi_channel_map_node);

    void accept_serializer(nlohmann::json& j, const NodeSerializer& serializer) const override;

    static const char* name();

private:
    void render_internal() override;

    static const char* get_label(size_t index);
    static const char* get_hidden_label(size_t index);

    static inline constexpr size_t sm_num_combo_items = 17;
    static const char*             sm_combo_items[sm_num_combo_items];

    std::shared_ptr<midi::ChannelMapNode> m_midi_channel_map_node;

    friend class NodeSerializer;
};

} // namespace mc
