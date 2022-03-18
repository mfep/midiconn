#pragma once
#include <string>
#include <vector>

namespace mc
{

struct License
{
    std::string m_library_name;
    std::string m_license_text;
};

extern std::vector<License> g_licenses;

}
