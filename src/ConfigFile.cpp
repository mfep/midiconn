#include "ConfigFile.hpp"

#include <fstream>

#include "nlohmann/json.hpp"
#include "spdlog/spdlog.h"

namespace mc
{
namespace
{

struct ConfigContent
{
    std::string m_last_preset_path;
    Theme m_theme;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ConfigContent, m_last_preset_path, m_theme);

} // namespace

ConfigFile::ConfigFile(const std::string_view exe_path)
{
    m_config_json_path = exe_path;
    m_config_json_path.replace_extension("json");
    if (!std::filesystem::exists(m_config_json_path))
    {
        spdlog::info("Config file does not exist at \"{}\"", m_config_json_path.string());
        return;
    }
    try
    {
        spdlog::info("Loading config file from \"{}\"", m_config_json_path.string());
        nlohmann::json j;
        {
            std::ifstream ifs(m_config_json_path);
            ifs >> j;
        }
        const auto config  = j.get<ConfigContent>();
        m_last_preset_path = config.m_last_preset_path;
        m_theme = config.m_theme;
    }
    catch (const std::exception& ex)
    {
        spdlog::info("Could not load config file. Reason: \"{}\"", ex.what());
    }
}

void ConfigFile::set_last_preset_path(const std::filesystem::path& path)
{
    m_last_preset_path = path;
    save_config_file();
}

void ConfigFile::set_theme(const Theme theme)
{
    m_theme = theme;
    save_config_file();
}

void ConfigFile::save_config_file() const
{
    spdlog::info("Saving config file to: \"{}\"", m_config_json_path.string());
    const ConfigContent config
    {
        m_last_preset_path.value_or("").string(),
        m_theme.value_or(Theme::Default)
    };
    const nlohmann::json j = config;
    std::ofstream ofs(m_config_json_path);
    ofs << j;
}

} // namespace mc
