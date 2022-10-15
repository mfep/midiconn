#pragma once
#include <filesystem>
#include <string>

#ifdef _WIN32

#define MAIN                                                                                       \
    int WinMain(                                                                                   \
        HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, PSTR /*argv*/, INT /*nCmdShow*/)

#else

#define MAIN int main(int /*argc*/, char** argv)

#endif

namespace mc::platform
{

std::filesystem::path get_config_dir();
std::filesystem::path get_logfile_path();
void                  open_logfile_external();

} // namespace mc::platform
