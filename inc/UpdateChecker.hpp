#pragma once

#include <atomic>
#include <optional>
#include <string>
#include <utility>
#include <variant>

namespace mc
{

struct UpdateChecker
{
    struct StatusNotSupported
    {
    };
    struct StatusFetching
    {
    };
    struct StatusError
    {
        std::string message;
    };
    struct StatusFetched
    {
        bool        is_latest_version;
        std::string latest_version_name;
    };
    using CheckResult =
        std::variant<StatusNotSupported, StatusFetching, StatusError, StatusFetched>;

    UpdateChecker();
    void        trigger_check();
    CheckResult get_latest_version() const;

private:
    bool             m_update_check_finished = false;
    CheckResult      m_latest_version_result = {};
    std::atomic_bool m_latest_version_written;
};

} // namespace mc
