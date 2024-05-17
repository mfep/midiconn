#pragma once
#include <string>
#include <string_view>

namespace mc
{

class PortNameDisplay final
{
public:
    PortNameDisplay(const bool initial_show_full_port_names);

    const bool& get_show_full_port_names() const { return m_show_full_port_names; }
    void        set_show_full_port_names(const bool value) { m_show_full_port_names = value; }

    std::string get_port_name(std::string_view full_name) const;

private:
    bool m_show_full_port_names;
};

} // namespace mc
