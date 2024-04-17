#pragma once

#include "ChannelMap.hpp"
#include "MidiGraph.hpp"

#include <string>

namespace mc::midi
{

class ChannelMapNode final : public Node
{
public:
    explicit ChannelMapNode(const ChannelMap& map = {});

    std::string_view name() override;
    bool             process(data_span data) override;

    const ChannelMap& map() const;
    ChannelMap&       map();

private:
    ChannelMap m_map;
};

} // namespace mc::midi
