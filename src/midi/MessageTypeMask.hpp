#pragma once

namespace mc::midi
{

struct MessageTypeMask
{
    bool m_sysex_enabled{true};
    bool m_time_enabled{true};
    bool m_sensing_enabled{true};
};


}
