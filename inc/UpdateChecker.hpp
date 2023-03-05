#pragma once

#include <atomic>
#include <string>
#include <utility>

namespace mc
{

struct UpdateChecker
{
    UpdateChecker();
    void show_latest_version_message();

private:
    bool                         m_update_check_finished = false;
    std::pair<bool, std::string> m_latest_version_result = {}; // is latest, latest version string
    std::atomic_bool             m_latest_version_written;
};

} // namespace mc
