#pragma once
#include <filesystem>
#include <string>
#include <vector>

#ifdef _WIN32

#define MAIN                                                                                       \
    int WinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, PSTR argv, INT /*nCmdShow*/)

#else

#define MAIN int main(int /*argc*/, char** argv)

#endif

struct SDL_Window;

namespace mc::platform
{

std::filesystem::path get_config_dir();
std::filesystem::path get_logfile_path();
void                  open_logfile_external();
void                  set_process_dpi_aware();
unsigned              get_window_dpi(SDL_Window* window);
template <class T>
std::filesystem::path get_arg(T args);

} // namespace mc::platform
