#pragma once

namespace mc
{

class ConfigFile;
class UpdateChecker;
class ThemeControl;

class WelcomeWindow final
{
public:
    WelcomeWindow(ConfigFile&         config,
                  UpdateChecker&      update_checker,
                  const ThemeControl& theme_control);

    void render();
    void show();

private:
    ConfigFile*         m_config;
    UpdateChecker*      m_update_checker;
    const ThemeControl* m_theme_control;
    bool                m_enabled;
};

} // namespace mc