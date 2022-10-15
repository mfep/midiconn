#include "PlatformUtils.hpp"

#include "ApplicationName.hpp"

#include <cstdlib>

namespace mc::platform
{

std::filesystem::path get_config_dir()
{
    // XDG_CONFIG_HOME is set in Flatpak
    if (const char* config_home = std::getenv("XDG_CONFIG_HOME"); config_home != nullptr)
    {
        return std::filesystem::path(config_home);
    }
    const auto config_dir =
        std::filesystem::path(std::getenv("HOME")) / "." MIDI_APPLICATION_NAME_SNAKE;
    std::filesystem::create_directories(config_dir);
    return config_dir;
}

void open_logfile_external()
{
    const std::string command = "xdg-open " + get_logfile_path().string();
    std::system(command.c_str());
}

} // namespace mc::platform
