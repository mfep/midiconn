#include "ConfigFile.hpp"

#include <fstream>

#include "nlohmann/json.hpp"
#include "spdlog/spdlog.h"

#include "PlatformUtils.hpp"
#include "Version.hpp"

namespace mc
{
namespace
{

struct ConfigContent
{
    std::string    m_last_preset_path;
    Theme          m_theme;
    InterfaceScale m_scale;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ConfigContent, m_last_preset_path, m_theme, m_scale);

} // namespace

ConfigFile::ConfigFile()
{
    m_config_json_path = platform::get_config_dir() / "config.json";
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
        m_theme            = config.m_theme;
        m_scale            = config.m_scale;
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

void ConfigFile::set_scale(const InterfaceScale scale)
{
    m_scale = scale;
    save_config_file();
}

void ConfigFile::save_config_file() const
{
    spdlog::info("Saving config file to: \"{}\"", m_config_json_path.string());
    const ConfigContent config{m_last_preset_path.value_or("").string(),
                               m_theme.value_or(Theme::Default),
                               m_scale.value_or(InterfaceScale::Scale_1_00)};
    nlohmann::json      j = config;
    j["version"]          = MC_FULL_VERSION;
    std::ofstream ofs(m_config_json_path);
    if (ofs.good())
    {
        ofs << j;
    }
    else
    {
        spdlog::warn("Could not save config file to \"{}\"", m_config_json_path.string());
    }
}

} // namespace mc
