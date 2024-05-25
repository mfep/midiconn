#pragma once

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

constexpr inline std::array<std::string_view, static_cast<size_t>(InterfaceScale::Size)>
    interface_scale_labels{"1.0", "1.25", "1.5", "1.75", "2.0", "Auto"};

class ScaleProvider
{
public:
    virtual ~ScaleProvider() = default;

    virtual InterfaceScale get_scale() const       = 0;
    virtual float          get_scale_value() const = 0;
};

} // namespace mc
