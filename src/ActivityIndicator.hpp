#pragma once
#include <chrono>

namespace mc
{

class ActivityIndicator
{
public:
    void render() const;
    void trigger();

private:
    std::chrono::time_point<std::chrono::system_clock> m_last_trigger;
};

} // namespace mc
