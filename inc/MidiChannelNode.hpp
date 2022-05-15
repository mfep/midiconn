#pragma once
#include "MidiInfo.hpp"
#include "Node.hpp"

namespace mc
{

class MidiChannelNode final : public Node
{
public:
    MidiChannelNode();

    void accept_serializer(nlohmann::json& j, const NodeSerializer& serializer) const;

private:
    void              render_internal() override;
    midi::channel_map transform_channel_map(const midi::channel_map& in_map) override;

    static const char* get_label(size_t index);
    static const char* get_hidden_label(size_t index);

    static inline constexpr size_t sm_num_combo_items = 17;
    static const char*             sm_combo_items[sm_num_combo_items];

    std::array<int, 16> m_channels;

    friend class NodeSerializer;
};

} // namespace mc
