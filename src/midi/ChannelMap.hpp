#pragma once

#include "fmt/format.h"

#include <array>
#include <cstdint>
#include <sstream>

namespace mc::midi
{

class ChannelMap
{
public:
    using channel_t                                  = std::uint8_t;
    static inline constexpr std::size_t num_channels = 16;
    static inline constexpr channel_t   no_channel   = static_cast<channel_t>(-1);
    using data_t                                     = std::array<channel_t, num_channels>;

    ChannelMap();

    channel_t     get(channel_t ch) const;
    void          set(channel_t ch, channel_t val);
    const data_t& data() const;
    data_t&       data();

    bool operator==(const ChannelMap& other) const;
    bool operator!=(const ChannelMap& other) const;

private:
    data_t m_map;
};

} // namespace mc::midi
