#include "Version.hpp"

#include <charconv>
#include <stdexcept>

namespace mc
{

Version Version::parse(std::string_view str)
{
    std::size_t start = 0;
    if (str[0] == 'v')
    {
        start = 1;
    }
    const auto first_dot  = str.find('.', start);
    const auto second_dot = str.find('.', first_dot + 1);
    const auto third_dot  = str.find('.', second_dot + 1);

    if (first_dot == std::string_view::npos || second_dot == std::string_view::npos)
    {
        throw std::runtime_error("Wrong version format");
    }

    int                major, minor, patch;
    std::optional<int> build_num;
    auto               result = std::from_chars(str.data() + start, str.data() + first_dot, major);
    if (result.ec != std::errc{})
    {
        throw std::runtime_error("Wrong version format");
    }

    result = std::from_chars(str.data() + first_dot + 1, str.data() + second_dot, minor);
    if (result.ec != std::errc{})
    {
        throw std::runtime_error("Wrong version format");
    }

    result = std::from_chars(
        str.data() + second_dot + 1, str.data() + std::min(third_dot, str.size()), patch);
    if (result.ec != std::errc{})
    {
        throw std::runtime_error("Wrong version format");
    }

    if (third_dot != std::string_view::npos)
    {
        int num;
        result = std::from_chars(str.data() + third_dot + 1, str.data() + str.size(), num);
        if (result.ec == std::errc{})
        {
            build_num = num;
        }
    }
    return Version(major, minor, patch, build_num);
}

const Version     g_current_version     = Version::parse(MC_FULL_VERSION);
const std::string g_current_version_str = fmt::format("{}", g_current_version);

} // namespace mc
