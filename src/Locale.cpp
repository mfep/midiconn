#include "Locale.hpp"

#include "ApplicationName.hpp"
#include "ConfigFile.hpp"

#include "libintl.h"
#include "portable-file-dialogs.h"
#include "spdlog/spdlog.h"

mc::Locale::Locale(ConfigFile& config_file) : m_config_file(&config_file)
{
    const auto config_locale = m_config_file->get_locale();
    spdlog::info("Setting locale to \"{}\"", config_locale);
    if (nullptr == std::setlocale(LC_ALL, config_locale.c_str()))
    {
        spdlog::error("Could not set locale");
    }
}

void mc::Locale::set_locale(std::string_view locale_code)
{
    m_config_file->set_locale(locale_code);
    pfd::message(MIDI_APPLICATION_NAME,
                 gettext("The change of display language is applied after restart"),
                 pfd::choice::ok)
        .result();
}

const std::vector<mc::Locale::LocaleSetting>& mc::Locale::get_supported_locales()
{
    static std::vector<LocaleSetting> locales{
        {"English (US)", "en_US.UTF-8"},
        {"Magyar",       "hu_HU.UTF-8"}
    };
    return locales;
}
