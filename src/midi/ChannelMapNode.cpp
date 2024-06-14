#include "ChannelMapNode.hpp"

#include "MessageView.hpp"

#include "fmt/format.h"

#include <type_traits>
#include <variant>

mc::midi::ChannelMapNode::ChannelMapNode(const ChannelMap& map) : m_map(map)
{
}

std::string_view mc::midi::ChannelMapNode::name()
{
    return "Channel Map Node";
}

bool mc::midi::ChannelMapNode::process(data_span data)
{
    MessageView         view(data);
    midi::tag_overloads visitor{[this](ChannelMessageViewTag, auto channel_message_view) {
                                    const auto current_channel = channel_message_view.get_channel();
                                    const auto mapped_channel  = m_map.get(current_channel);
                                    if (mapped_channel != ChannelMap::no_channel)
                                    {
                                        channel_message_view.set_channel(mapped_channel);
                                        return true;
                                    }
                                    return false;
                                },
                                [](auto /*any_tag*/, auto /*any_message_view*/) {
                                    return true;
                                }};
    return std::visit(visitor, view.parse());
}

const mc::midi::ChannelMap& mc::midi::ChannelMapNode::map() const
{
    return m_map;
}

mc::midi::ChannelMap& mc::midi::ChannelMapNode::map()
{
    return m_map;
}
