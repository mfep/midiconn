#pragma once
#include <string>

std::string get_exe_path(char** argv);

#ifdef _WIN32

#define MAIN                                                                                       \
    int WinMain(                                                                                   \
        HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, PSTR /*argv*/, INT /*nCmdShow*/)
#define GET_EXE_PATH() get_exe_path(nullptr)

#else

#define MAIN           int main(int /*argc*/, char** argv)
#define GET_EXE_PATH() get_exe_path(argv)

#endif
