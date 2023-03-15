#include "UpdateChecker.hpp"

namespace mc
{

UpdateChecker::UpdateChecker() = default;

void UpdateChecker::trigger_check()
{
}

UpdateChecker::CheckResult UpdateChecker::get_latest_version() const
{
    return StatusNotSupported{};
}

} // namespace mc
