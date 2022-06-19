#pragma once

#include <array>
#include <string_view>

#include "imnodes.h"

namespace mc
{

class ConfigFile;

enum class Theme
{
    Dark,
    Light,
    Classic,
    Default = Dark
};

enum class InterfaceScale
{
    Scale_1_00,
    Scale_1_25,
    Scale_1_50,
    Scale_1_75,
    Scale_2_00,
    Size
};

constexpr inline std::array<std::string_view, static_cast<size_t>(InterfaceScale::Size)>
    interface_scale_labels{"1.0", "1.25", "1.5", "1.75", "2.0"};

class ThemeControl final
{
public:
    ThemeControl(ConfigFile& config);

    void           set_theme(const Theme theme);
    Theme          get_theme() const { return m_current_theme; }
    void           set_scale(const InterfaceScale scale);
    InterfaceScale get_scale() const { return m_scale; }

    // Call this outside of ImGui's NewFrame() and Render()
    void update_scale_if_needed();

private:
    void set_theme_internal(const Theme theme);
    void set_scale_internal(const float scale);

    ConfigFile*    m_config;
    Theme          m_current_theme{};
    InterfaceScale m_scale{};
    InterfaceScale m_new_scale{};
    ImNodesStyle   m_original_nodes_style;
};

} // namespace mc
