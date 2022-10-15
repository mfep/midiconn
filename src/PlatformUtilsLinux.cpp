#include <string>

#include "PlatformUtils.hpp"

namespace mc::platform
{

std::string get_exe_path(char** argv)
{
    return *argv;
}

std::filesystem::path get_config_dir()
{
    return {};
}

} // namespace mc::platform
