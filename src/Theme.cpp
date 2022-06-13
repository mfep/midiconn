#include "Theme.hpp"

#include "imgui.h"
#include "imnodes.h"

#include "ConfigFile.hpp"

namespace mc
{

ThemeControl::ThemeControl(ConfigFile& config) : m_config(&config)
{
    set_theme_internal(m_config->get_theme().value_or(Theme::Default));
}

void ThemeControl::set_theme(const Theme theme)
{
    set_theme_internal(theme);
    m_config->set_theme(theme);
}

void ThemeControl::set_theme_internal(const Theme theme)
{
    switch (m_current_theme = theme)
    {
    case Theme::Dark:
        ImGui::StyleColorsDark();
        ImNodes::StyleColorsDark();
        break;
    case Theme::Light:
        ImGui::StyleColorsLight();
        ImNodes::StyleColorsLight();
        break;
    case Theme::Classic:
        ImGui::StyleColorsClassic();
        ImNodes::StyleColorsClassic();
        break;
    default:
        break;
    }
}

} // namespace mc
