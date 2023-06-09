#include "PlatformUtils.hpp"

#include <cassert>
#include <cstdlib>

#include "spdlog/spdlog.h"

#include "ApplicationName.hpp"
#include "Utils.hpp"
#include "Version.hpp"

#if defined(MC_CHECK_FOR_UPDATES)
#include "curl/curl.h"
#endif

namespace mc::platform
{

std::filesystem::path get_config_dir()
{
    // XDG_CONFIG_HOME is set in Flatpak
    if (const char* config_home = std::getenv("XDG_CONFIG_HOME"); config_home != nullptr)
    {
        return std::filesystem::path(config_home);
    }
    const auto config_dir =
        std::filesystem::path(std::getenv("HOME")) / "." MIDI_APPLICATION_NAME_SNAKE;
    std::filesystem::create_directories(config_dir);
    return config_dir;
}

void open_logfile_external()
{
    const std::string command = "xdg-open " + utils::path_to_utf8str(get_logfile_path());
    const auto        result  = std::system(command.c_str());
    (void)result;
}

void set_process_dpi_aware()
{
    spdlog::info("DPI aware process is not supported on the current platform.");
}

unsigned get_window_dpi(SDL_Window* /*window*/)
{
    spdlog::info("Querying window DPI is not supported on the current platform.");
    return 96;
}

template <>
std::filesystem::path get_cli_path(int argc, char** argv)
{
    if (argc < 2)
    {
        return {};
    }
    return argv[1];
}

namespace
{

[[maybe_unused]] size_t write_fun(void* contents, size_t size, size_t nmemb, void* userptr)
{
    assert(size == 1);
    (void)size;
    auto             full_response = static_cast<std::string*>(userptr);
    std::string_view current_chunk(static_cast<char*>(contents), nmemb);
    full_response->append(current_chunk);
    return nmemb;
}

} // namespace

std::string get_request(std::string_view url)
{
#if defined(MC_CHECK_FOR_UPDATES)
    CURL* curl_handle = curl_easy_init();
    if (curl_handle == nullptr)
    {
        return {};
    }
    utils::final_action cleanup([=] {
        curl_easy_cleanup(curl_handle);
    });
    curl_easy_setopt(curl_handle, CURLOPT_URL, url.data());
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "midiconn/" MC_FULL_VERSION);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_fun);
    std::string ret;
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &ret);
    const auto result = curl_easy_perform(curl_handle);
    if (result == CURLE_OK)
    {
        return ret;
    }
#else
    (void)url;
#endif
    return {};
}

} // namespace mc::platform
