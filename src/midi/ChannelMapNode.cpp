#include "ChannelMapNode.hpp"

#include "MessageView.hpp"

#include "fmt/format.h"

mc::midi::ChannelMapNode::ChannelMapNode(const ChannelMap& map) : m_map(map)
{
}

std::string_view mc::midi::ChannelMapNode::name()
{
    return "Channel Map Node";
}

bool mc::midi::ChannelMapNode::process(data_span data)
{
    MessageView view(data);
    if (!view.is_system())
    {
        const auto current_channel = view.get_channel();
        const auto mapped_channel  = m_map.get(current_channel);
        if (mapped_channel != ChannelMap::no_channel)
        {
            view.set_channel(mapped_channel);
            return true;
        }
        return false;
    }
    return true;
}

const mc::midi::ChannelMap& mc::midi::ChannelMapNode::map() const
{
    return m_map;
}

mc::midi::ChannelMap& mc::midi::ChannelMapNode::map()
{
    return m_map;
}
