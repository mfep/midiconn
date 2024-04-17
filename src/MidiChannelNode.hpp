#pragma once

#include <functional>

#include "ActivityIndicator.hpp"
#include "Node.hpp"
#include "midi/ChannelMapNode.hpp"

namespace mc
{

class MidiChannelNode final : public Node, private midi::GraphObserver
{
public:
    explicit MidiChannelNode(std::function<float()> get_scale);
    ~MidiChannelNode();

    void accept_serializer(nlohmann::json& j, const NodeSerializer& serializer) const override;

protected:
    midi::Node* get_midi_node() override;

private:
    void render_internal() override;
    void message_processed(std::span<const unsigned char> message_bytes) override;
    void message_received(std::span<const unsigned char> message_bytes) override;

    static const char* get_label(size_t index);
    static const char* get_hidden_label(size_t index);

    static inline constexpr size_t sm_num_combo_items = 17;
    static const char*             sm_combo_items[sm_num_combo_items];

    std::function<float()> m_get_scale;
    midi::ChannelMapNode   m_midi_channel_map_node;

    ActivityIndicator m_input_indicator;
    ActivityIndicator m_output_indicator;

    friend class NodeSerializer;
};

} // namespace mc
