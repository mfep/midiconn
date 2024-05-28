#include "UpdateChecker.hpp"

#include "nlohmann/json.hpp"
#include "spdlog/spdlog.h"

#include <chrono>
#include <future>

#include "PlatformUtils.hpp"
#include "Version.hpp"

namespace mc
{

UpdateChecker::UpdateChecker()
{
    trigger_check();
}

void UpdateChecker::trigger_check()
{
    m_latest_version_written = false;
    std::thread([this] {
        using namespace std::chrono_literals;

        CheckResult result;
        try
        {
            const auto response_text = platform::get_request(
                "https://api.github.com/repos/mfep/midiconn/releases/latest");
            const auto j           = nlohmann::json::parse(response_text);
            const auto tag_name    = j.at("tag_name").get<std::string>();
            const auto tag_version = Version::parse(tag_name);
            const bool is_latest   = tag_version <= g_current_version;
            result                 = StatusFetched{is_latest, tag_name};
        }
        catch (std::exception& ex)
        {
            result = StatusError{ex.what()};
        }
        m_latest_version_result  = result;
        m_latest_version_written = true;
    }).detach();
}

UpdateChecker::CheckResult UpdateChecker::get_latest_version() const
{
    if (m_latest_version_written)
    {
        return m_latest_version_result;
    }
    return StatusFetching{};
}

} // namespace mc
