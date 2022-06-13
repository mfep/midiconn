#pragma once

#include <filesystem>
#include <optional>
#include <string_view>

namespace mc
{

class ConfigFile final
{
public:
    explicit ConfigFile(const std::string_view exe_path);

    std::optional<std::filesystem::path> get_last_preset_path() const
    {
        return m_last_preset_path;
    };
    void set_last_preset_path(const std::filesystem::path& path);

private:
    void save_config_file() const;

    std::filesystem::path m_config_json_path;
    std::optional<std::filesystem::path> m_last_preset_path;
};

} // namespace mc
