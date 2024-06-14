#include "PortNameDisplay.hpp"

#include <regex>

namespace mc
{

PortNameDisplay::PortNameDisplay(const bool initial_show_full_port_names)
    : m_show_full_port_names(initial_show_full_port_names)
{
}

std::string PortNameDisplay::get_port_name(std::string_view full_name) const
{
    const std::regex re(R"(^(.+):(\1.*) \d+:\d+$)");
    std::smatch      results;
    std::string      full_name_copy(full_name);
    if (!m_show_full_port_names && std::regex_match(full_name_copy, results, re))
    {
        return results[2].str();
    }
    else
    {
        return full_name_copy;
    }
}

} // namespace mc
