#include "ConfigFile.hpp"

#include <fstream>

#include "nlohmann/json.hpp"
#include "spdlog/spdlog.h"

#include "PlatformUtils.hpp"
#include "Version.hpp"

namespace mc
{

ConfigFile::ConfigFile()
{
    m_config_json_path = platform::get_config_dir() / "config.json";
    if (!std::filesystem::exists(m_config_json_path))
    {
        spdlog::info("Config file does not exist at \"{}\"", m_config_json_path.string());

        // default values
        m_scale                = InterfaceScale::Auto;
        m_show_full_port_names = false;
        m_theme                = Theme::Default;
        m_show_welcome         = true;
        save_config_file();
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
        bool       save_defaults = false;
        const auto try_load_item =
            [&](auto& item, auto default_value, const std::string_view name) {
                if (!j.contains(name))
                {
                    save_defaults = true;
                    item          = default_value;
                }
                else
                {
                    j[name.data()].get_to(item);
                }
            };
        try_load_item(m_scale, InterfaceScale::Auto, "scale");
        try_load_item(m_show_full_port_names, false, "show_full_port_names");
        try_load_item(m_theme, Theme::Default, "theme");
        try_load_item(m_show_welcome, true, "show_welcome");
        if (j.contains("last_preset_path"))
        {
            m_last_preset_path = j["last_preset_path"].get<std::string>();
        }
        if (save_defaults)
        {
            spdlog::info("Updating config file with defaults.");
            save_config_file();
        }
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

void ConfigFile::set_show_port_full_names(const bool value)
{
    m_show_full_port_names = value;
    save_config_file();
}

void ConfigFile::set_show_welcome(const bool value)
{
    m_show_welcome = value;
    save_config_file();
}

void ConfigFile::save_config_file() const
{
    spdlog::info("Saving config file to: \"{}\"", m_config_json_path.string());
    nlohmann::json j;
    j["version"]              = g_current_version_str;
    j["scale"]                = m_scale;
    j["theme"]                = m_theme;
    j["show_full_port_names"] = m_show_full_port_names;
    j["show_welcome"]         = m_show_welcome;
    if (m_last_preset_path)
    {
        j["last_preset_path"] = m_last_preset_path->string();
    }
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
