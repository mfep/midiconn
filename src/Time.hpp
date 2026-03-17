#pragma once

#include <chrono>

namespace mc
{

using clock_t      = std::chrono::system_clock;
using time_point_t = std::chrono::time_point<clock_t>;

} // namespace mc
