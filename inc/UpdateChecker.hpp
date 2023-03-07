#pragma once

#include <atomic>
#include <string>
#include <utility>

namespace mc
{

struct ConfigFile;

struct UpdateChecker
{
    UpdateChecker(ConfigFile& config);
    void show_latest_version_message();
    void trigger_check();
    bool update_check_supported() const;

private:
    bool                         m_update_check_finished = false;
    std::pair<bool, std::string> m_latest_version_result = {}; // is latest, latest version string
    std::atomic_bool             m_latest_version_written;
    ConfigFile*                  m_config;
};

} // namespace mc
