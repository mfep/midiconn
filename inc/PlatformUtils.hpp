#pragma once
#include <filesystem>
#include <string>

#ifdef _WIN32

#define MAIN                                                                                       \
    int WinMain(                                                                                   \
        HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, PSTR /*argv*/, INT /*nCmdShow*/)
#define GET_EXE_PATH() ::mc::platform::get_exe_path(nullptr)

#else

#define MAIN           int main(int /*argc*/, char** argv)
#define GET_EXE_PATH() ::mc::platform::get_exe_path(argv)

#endif

namespace mc::platform
{

std::string           get_exe_path(char** argv);
std::filesystem::path get_config_dir();

} // namespace mc::platform
