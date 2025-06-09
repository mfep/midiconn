#pragma once

#include "Intl.hpp"

#include <array>
#include <string_view>

namespace mc
{

enum class InterfaceScale
{
    Scale_1_00,
    Scale_1_25,
    Scale_1_50,
    Scale_1_75,
    Scale_2_00,
    Auto,
    Size,
    Undefined
};

inline std::array<std::string_view, static_cast<size_t>(InterfaceScale::Size)>
get_interface_scale_labels()
{
    using namespace std::string_view_literals;
    return std::array<std::string_view, static_cast<size_t>(InterfaceScale::Size)>{
        "1.0"sv,
        "1.25"sv,
        "1.5"sv,
        "1.75"sv,
        "2.0"sv,
        // Translators: Automatic scaling of the interface
        gettext("Auto")};
}

class ScaleProvider
{
public:
    virtual ~ScaleProvider() = default;

    virtual InterfaceScale get_scale() const       = 0;
    virtual float          get_scale_value() const = 0;
};

} // namespace mc
