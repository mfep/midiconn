#pragma once

namespace mc
{

class ConfigFile;
class UpdateChecker;

class WelcomeWindow final
{
public:
    WelcomeWindow(ConfigFile& config, UpdateChecker& update_checker);

    void render();
    void show();

private:
    ConfigFile*    m_config;
    UpdateChecker* m_update_checker;
    bool           m_enabled;
};

} // namespace mc