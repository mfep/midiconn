#pragma once

namespace mc
{

enum class Theme
{
    Dark,
    Light,
    Classic
};

class ThemeControl final
{
public:
    void  set_theme(const Theme theme);
    Theme get_theme() const { return m_current_theme; }

private:
    Theme m_current_theme{};
};

} // namespace mc
