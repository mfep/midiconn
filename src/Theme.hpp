#pragma once

#include <array>
#include <optional>
#include <string_view>

#include "ScaleProvider.hpp"

#include "imnodes.h"

struct SDL_Window;

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

class ThemeControl final : public ScaleProvider
{
public:
    ThemeControl(ConfigFile& config, SDL_Window* window);

    void           set_theme(const Theme theme);
    Theme          get_theme() const { return m_current_theme; }
    void           set_scale(const InterfaceScale scale);
    InterfaceScale get_scale() const override { return m_scale; }
    float          get_scale_value() const override { return calculate_scale_value(m_scale); }

    // Call this outside of ImGui's NewFrame() and Render()
    void update_scale_if_needed();

private:
    void  set_theme_internal(const Theme theme);
    void  set_scale_internal(const InterfaceScale scale);
    float calculate_scale_value(const InterfaceScale scale) const;
    float get_auto_interface_scale() const;

    ConfigFile*                  m_config;
    SDL_Window*                  m_window;
    Theme                        m_current_theme{};
    InterfaceScale               m_scale{InterfaceScale::Undefined};
    InterfaceScale               m_new_scale{};
    ImNodesStyle                 m_original_nodes_style;
    mutable std::optional<float> m_detected_interface_scale;
};

} // namespace mc
