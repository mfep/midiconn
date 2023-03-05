#pragma once

namespace mc
{

class ConfigFile;

class WelcomeWindow final
{
public:
    WelcomeWindow(ConfigFile& config);

    void render();

private:
    ConfigFile* m_config;
    bool        m_enabled;
};

} // namespace mc