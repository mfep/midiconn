#include "PlatformUtils.hpp"

#include "ApplicationName.hpp"

namespace mc::platform
{

std::filesystem::path get_logfile_path()
{
    return get_config_dir() / MIDI_APPLICATION_NAME_SNAKE ".log";
}

} // namespace mc::platform
