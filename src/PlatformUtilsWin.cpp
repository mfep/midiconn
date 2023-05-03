#include "PlatformUtils.hpp"

#include <array>
#include <codecvt>
#include <locale>
#include <sstream>
#include <stdexcept>
#include <string>

#include "SDL2/SDL.h"
#include "SDL2/SDL_syswm.h"
#include "spdlog/spdlog.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <shellapi.h>
#include <shellscalingapi.h>
#include <shlobj_core.h>
#include <winuser.h>

#include <winrt/base.h>
#include <winrt/windows.foundation.collections.h>
#include <winrt/windows.foundation.h>
#include <winrt/windows.web.http.h>
#include <winrt/windows.web.http.headers.h>

#include "ApplicationName.hpp"
#include "Version.hpp"

namespace mc::platform
{

std::filesystem::path get_config_dir()
{
    std::array<char, MAX_PATH> appdata_path_chars{};
    if (!SHGetSpecialFolderPath(0, appdata_path_chars.data(), CSIDL_APPDATA, false))
    {
        throw std::runtime_error("Cannot get config file location");
    }
    const auto config_dir = std::filesystem::path(appdata_path_chars.data()) /
                            MIDI_APPLICATION_ORG / MIDI_APPLICATION_NAME_SNAKE;
    std::filesystem::create_directories(config_dir);
    return config_dir;
}

void open_logfile_external()
{
    if (reinterpret_cast<INT_PTR>(ShellExecute(nullptr,
                                               "open",
                                               get_logfile_path().string().c_str(),
                                               nullptr,
                                               nullptr,
                                               SW_SHOWNORMAL)) <= 32)
    {
        throw std::runtime_error("Could not open logfile.");
    }
}

void set_process_dpi_aware()
{
    if (const auto result = SetProcessDpiAwareness(PROCESS_SYSTEM_DPI_AWARE); result != S_OK)
    {
        spdlog::warn("Could not set process DPI aware ({})", result);
    }
}

unsigned get_window_dpi(SDL_Window* window)
{
    SDL_SysWMinfo window_info{};
    const bool    result = SDL_GetWindowWMInfo(window, &window_info);
    if (!result)
    {
        throw std::runtime_error("Could not query window info");
    }
    const auto window_handle = window_info.info.win.window;
    return GetDpiForWindow(window_handle);
}

template <>
std::filesystem::path get_cli_path(PSTR arg)
{
    std::filesystem::path path(arg);
    if (!path.empty() && !std::filesystem::is_regular_file(path))
    {
        std::stringstream sstream;
        sstream << "Path \"" << arg << "\" is not a file.";
        throw std::runtime_error(sstream.str());
    }
    return path;
}

std::string get_request(std::string_view url)
{
    using namespace winrt;
    using namespace Windows::Foundation;
    using namespace Windows::Foundation::Collections;
    using namespace Windows::Web::Http;
    using namespace Windows::Web::Http::Headers;

    const HttpClient                 client;
    const HttpProductInfoHeaderValue header(L"midiconn/" MC_FULL_VERSION);
    client.DefaultRequestHeaders().UserAgent().Append(header);
    const Uri  uri(std::wstring(url.begin(), url.end()));
    const auto response = client.GetAsync(uri).get();

    if (!response.IsSuccessStatusCode())
    {
        spdlog::warn("Could not perform GET request to '{}'", url);
        throw std::runtime_error("Could not perform GET request");
    }

    const winrt::hstring response_text = response.Content().ReadAsStringAsync().get();
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.to_bytes(response_text.begin(), response_text.end());
}

} // namespace mc::platform
