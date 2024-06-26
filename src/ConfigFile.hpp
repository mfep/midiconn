#pragma once

#include <filesystem>
#include <optional>
#include <string_view>

#include "Theme.hpp"

namespace mc
{

class ConfigFile final
{
public:
    ConfigFile();

    std::optional<std::filesystem::path> get_last_preset_path() const
    {
        return m_last_preset_path;
    };
    Theme              get_theme() const { return m_theme; }
    InterfaceScale     get_scale() const { return m_scale; }
    bool               get_show_full_port_names() const { return m_show_full_port_names; }
    bool               get_show_welcome() const { return m_show_welcome; }
    const std::string& get_locale() const { return m_locale; }
    void               set_last_preset_path(const std::filesystem::path& path);
    void               set_theme(const Theme theme);
    void               set_scale(const InterfaceScale scale);
    void               set_show_port_full_names(const bool value);
    void               set_show_welcome(const bool value);
    void               set_locale(std::string_view value);

private:
    void save_config_file() const;

    std::filesystem::path                m_config_json_path;
    std::optional<std::filesystem::path> m_last_preset_path;
    Theme                                m_theme;
    InterfaceScale                       m_scale;
    bool                                 m_show_full_port_names;
    bool                                 m_show_welcome;
    std::string                          m_locale;
};

} // namespace mc
