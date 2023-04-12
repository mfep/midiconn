#pragma once
#include <filesystem>
#include <string>
#include <string_view>
#include <vector>

#ifdef _WIN32

#define MC_MAIN                                                                                    \
    int WinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, PSTR argv, INT /*nCmdShow*/)
#define MC_GET_CLI_PATH ::mc::platform::get_cli_path(argv)

#else

#define MC_MAIN         int main(int argc, char** argv)
#define MC_GET_CLI_PATH ::mc::platform::get_cli_path(argc, argv)

#endif

struct SDL_Window;

namespace mc::platform
{

std::filesystem::path get_config_dir();
std::filesystem::path get_logfile_path();
void                  open_logfile_external();
void                  set_process_dpi_aware();
unsigned              get_window_dpi(SDL_Window* window);
template <class... Ts>
std::filesystem::path get_cli_path(Ts...);
std::string get_request(std::string_view url);

} // namespace mc::platform
