#pragma once

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

class ThemeControl final
{
public:
    ThemeControl(ConfigFile& config);

    void  set_theme(const Theme theme);
    Theme get_theme() const { return m_current_theme; }
    void  set_scale(const float scale);
    float get_scale() const { return m_scale; }

    // Call this outside of ImGui's NewFrame() and Render()
    void update_scale_if_needed();

private:
    void set_theme_internal(const Theme theme);
    void set_scale_internal(const float scale);

    ConfigFile*  m_config;
    Theme        m_current_theme{};
    float        m_scale{1.F};
    float        m_new_scale{0.F};
    ImNodesStyle m_original_nodes_style;
};

} // namespace mc
