#include "PlatformUtils.hpp"

#include <array>
#include <stdexcept>
#include <string>

#define WIN32_LEAN_AND_MEAN
#include "shlobj_core.h"
#include <Windows.h>

#include "ApplicationName.hpp"

namespace mc::platform
{

std::filesystem::path get_config_dir()
{
    std::array<char, MAX_PATH> appdata_path_chars{};
    if (!SHGetSpecialFolderPath(0, appdata_path_chars.data(), CSIDL_APPDATA, false))
    {
        throw std::runtime_error("Cannot get config file location");
    }
    const auto config_dir = std::filesystem::path(appdata_path_chars.data()) /
                            MIDI_APPLICATION_ORG / MIDI_APPLICATION_NAME_SNAKE;
    std::filesystem::create_directories(config_dir);
    return config_dir;
}

} // namespace mc::platform
