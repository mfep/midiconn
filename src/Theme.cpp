#include "Theme.hpp"

#include "imgui.h"
#include "spdlog/spdlog.h"

#include "ConfigFile.hpp"

bool ImGui_ImplSDLRenderer_CreateFontsTexture();

namespace mc
{
namespace
{

float calculate_scale_value(const InterfaceScale scale)
{
    return 1 + 0.25F * static_cast<int>(scale);
}

} // namespace

ThemeControl::ThemeControl(ConfigFile& config) : m_config(&config)
{
    m_original_nodes_style = ImNodes::GetStyle();
    set_theme_internal(m_config->get_theme().value_or(Theme::Default));

    m_new_scale = m_config->get_scale().value_or(InterfaceScale::Scale_1_00);
    update_scale_if_needed();
}

void ThemeControl::set_theme(const Theme theme)
{
    set_theme_internal(theme);
    m_config->set_theme(theme);
}

void ThemeControl::set_scale(const InterfaceScale scale)
{
    m_new_scale = scale;
}

void ThemeControl::update_scale_if_needed()
{
    if (m_new_scale != m_scale)
    {
        set_scale_internal(m_new_scale);
        m_config->set_scale(m_new_scale);
        m_scale = m_new_scale;
    }
}

void ThemeControl::set_theme_internal(const Theme theme)
{
    spdlog::info("Setting application theme to {}", static_cast<int>(theme));
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

void ThemeControl::set_scale_internal(const InterfaceScale scale)
{
    const float scale_value = calculate_scale_value(scale);
    spdlog::info("Setting application scale to {}", scale_value);
    ImGui::GetIO().Fonts->Clear();
    ImGui::GetIO().Fonts->AddFontFromFileTTF("DroidSans.ttf", 16 * scale_value);

    // workaround, otherwise the fonts won't rebuild properly
    ImGui_ImplSDLRenderer_CreateFontsTexture();

    auto& current_nodes_style         = ImNodes::GetStyle();
    current_nodes_style.GridSpacing   = m_original_nodes_style.GridSpacing * scale_value;
    current_nodes_style.LinkThickness = m_original_nodes_style.LinkThickness * scale_value;
    current_nodes_style.NodeBorderThickness =
        m_original_nodes_style.NodeBorderThickness * scale_value;
    current_nodes_style.NodeCornerRounding =
        m_original_nodes_style.NodeCornerRounding * scale_value;
    current_nodes_style.NodePadding      = {m_original_nodes_style.NodePadding.x * scale_value,
                                            m_original_nodes_style.NodePadding.y * scale_value};
    current_nodes_style.PinCircleRadius  = m_original_nodes_style.PinCircleRadius * scale_value;
    current_nodes_style.PinLineThickness = m_original_nodes_style.PinLineThickness * scale_value;
}

} // namespace mc
