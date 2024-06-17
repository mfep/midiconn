#pragma once

#include <string>
#include <vector>

namespace mc
{
class ConfigFile;

class Locale final
{
public:
    struct LocaleSetting
    {
        std::string display_name;
        std::string locale_code;
    };

    explicit Locale(ConfigFile& config_file);
    void set_locale(std::string_view locale_code);

    static const std::vector<LocaleSetting>& get_supported_locales();

private:
    ConfigFile* m_config_file;
};

} // namespace mc
