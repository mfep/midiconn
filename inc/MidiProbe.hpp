#pragma once
#include <optional>
#include <vector>

#include "MidiInfo.hpp"

namespace mc
{

class MidiProbe final
{
public:
    MidiProbe() = delete;

    static std::vector<midi::InputInfo>  get_inputs();
    static std::vector<midi::OutputInfo> get_outputs();

    static std::optional<midi::InputInfo>  get_valid_input(const std::string& input_name);
    static std::optional<midi::OutputInfo> get_valid_output(const std::string& output_name);

    static std::optional<std::string> get_valid_input_port_name(unsigned id);
    static std::optional<std::string> get_valid_output_port_name(unsigned id);
};

} // namespace mc
