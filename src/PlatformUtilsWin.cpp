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

std::string get_exe_path(char**)
{
    constexpr DWORD filename_length = 512;
    CHAR            filename[filename_length];
    const auto      buffer_length = GetModuleFileNameA(nullptr, filename, filename_length);
    if (filename_length == buffer_length)
    {
        throw std::runtime_error("Could not get current exe's path");
    }
    return std::string(filename, buffer_length);
}

std::filesystem::path get_config_dir()
{
    std::array<char, MAX_PATH> appdata_path_chars{};
    if (!SHGetSpecialFolderPath(0, appdata_path_chars.data(), CSIDL_APPDATA, false))
    {
        throw std::runtime_error("Cannot get config file location");
    }
    const std::filesystem::path appdata_path(appdata_path_chars.data());
    return appdata_path / MIDI_APPLICATION_ORG / MIDI_APPLICATION_NAME_SNAKE;
}

} // namespace mc::platform
