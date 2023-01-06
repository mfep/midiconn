#include "PortNameDisplay.hpp"

#include <regex>

namespace mc
{
namespace
{

std::string abbreviate_port_name(const std::string& port_name)
{
    const std::regex re(R"(^(.+):(\1.*) \d+:\d+$)");
    std::smatch      results;
    if (std::regex_match(port_name, results, re))
    {
        return results[2].str();
    }
    else
    {
        return port_name;
    }
}

} // namespace

PortNameDisplay::PortNameDisplay(const bool initial_show_full_port_names)
    : m_show_full_port_names(initial_show_full_port_names)
{
}

std::string PortNameDisplay::get_port_name(const midi::InputInfo& input_info) const
{
    if (m_show_full_port_names)
    {
        return input_info.m_name;
    }
    return abbreviate_port_name(input_info.m_name);
}

std::string PortNameDisplay::get_port_name(const midi::OutputInfo& output_info) const
{
    if (m_show_full_port_names)
    {
        return output_info.m_name;
    }
    return abbreviate_port_name(output_info.m_name);
}

} // namespace mc
