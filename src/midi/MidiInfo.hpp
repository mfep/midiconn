#pragma once
#include <array>
#include <cstdlib>
#include <string>

namespace mc::midi
{

struct InputInfo final
{
    size_t      m_id{};
    std::string m_name;
};

struct OutputInfo final
{
    size_t      m_id{};
    std::string m_name;
};

using channel_map               = std::array<char, 16>;
constexpr char disabled_channel = -1;

} // namespace mc::midi
