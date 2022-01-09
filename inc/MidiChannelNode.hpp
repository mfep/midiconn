#pragma once
#include "Node.hpp"

namespace mc
{

class MidiChannelNode final : public Node
{
public:
    MidiChannelNode();

private:
    void render_internal() override;
    channel_map transform_channel_map(const channel_map& in_map) override;

    static const char* get_label(size_t index);
    static const char* get_hidden_label(size_t index);

    static inline constexpr size_t sm_num_combo_items = 17;
    static const char* sm_combo_items[sm_num_combo_items];

    std::array<int, 16> m_channels;
};

}
