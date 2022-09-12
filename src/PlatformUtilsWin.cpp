#include "PlatformUtils.hpp"

#include <stdexcept>
#include <string>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

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
