#pragma once
#include <string>

#include "midi/MidiInfo.hpp"

namespace mc
{

class PortNameDisplay final
{
public:
    PortNameDisplay(const bool initial_show_full_port_names);

    const bool& get_show_full_port_names() const { return m_show_full_port_names; }
    void        set_show_full_port_names(const bool value) { m_show_full_port_names = value; }

    std::string get_port_name(const midi::InputInfo& input_info) const;
    std::string get_port_name(const midi::OutputInfo& output_info) const;

private:
    bool m_show_full_port_names;
};

} // namespace mc
