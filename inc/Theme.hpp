#pragma once

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

private:
    void set_theme_internal(const Theme theme);

    ConfigFile* m_config;
    Theme       m_current_theme{};
};

} // namespace mc
