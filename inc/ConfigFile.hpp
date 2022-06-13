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
    explicit ConfigFile(const std::string_view exe_path);

    const std::optional<std::filesystem::path>& get_last_preset_path() const
    {
        return m_last_preset_path;
    };
    const std::optional<Theme>& get_theme() const
    {
        return m_theme;
    }
    void set_last_preset_path(const std::filesystem::path& path);
    void set_theme(const Theme theme);

private:
    void save_config_file() const;

    std::filesystem::path m_config_json_path;
    std::optional<std::filesystem::path> m_last_preset_path;
    std::optional<Theme> m_theme;
};

} // namespace mc
