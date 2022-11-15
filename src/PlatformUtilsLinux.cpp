#include "PlatformUtils.hpp"

#include <cstdlib>

#include "spdlog/spdlog.h"

#include "ApplicationName.hpp"

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

void set_process_dpi_aware()
{
    spdlog::info("DPI aware process is not supported on the current platform.");
}

unsigned get_window_dpi(SDL_Window* window)
{
    spdlog::info("Queriying window DPI is not supported on the current platform.");
}

} // namespace mc::platform
