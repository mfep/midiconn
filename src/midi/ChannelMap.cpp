#include "ChannelMap.hpp"

#include <cassert>
#include <numeric>

mc::midi::ChannelMap::ChannelMap()
{
    std::iota(m_map.begin(), m_map.end(), 0);
}

mc::midi::ChannelMap::channel_t mc::midi::ChannelMap::get(channel_t ch) const
{
    assert(ch < num_channels || ch == no_channel);
    return m_map[ch];
}

void mc::midi::ChannelMap::set(channel_t ch, channel_t val)
{
    assert(ch < num_channels || ch == no_channel);
    m_map[ch] = val;
}

const mc::midi::ChannelMap::data_t& mc::midi::ChannelMap::data() const
{
    return m_map;
}

mc::midi::ChannelMap::data_t& mc::midi::ChannelMap::data()
{
    return m_map;
}

bool mc::midi::ChannelMap::operator==(const ChannelMap& other) const
{
    return m_map == other.m_map;
}

bool mc::midi::ChannelMap::operator!=(const ChannelMap& other) const
{
    return !(*this == other);
}
