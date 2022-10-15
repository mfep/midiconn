#define SDL_MAIN_HANDLED

#include "SDL2/SDL.h"
#include "SDL2/SDL_syswm.h"
#include "backends/imgui_impl_sdl.h"
#include "backends/imgui_impl_sdlrenderer.h"
#include "imgui.h"
#include "imnodes.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"

#include "Application.hpp"
#include "KeyboardShotcutAggregator.hpp"
#include "PlatformUtils.hpp"

#if !SDL_VERSION_ATLEAST(2, 0, 17)
#error This backend requires SDL 2.0.17+ because of SDL_RenderGeometry() function
#endif

MAIN
{
    // Setup spdlog
    static constexpr size_t max_logfile_size = 5 * 1024 * 1024; // 5 MiB
    static constexpr size_t num_logfiles     = 2;
    auto                    rotating_logger  = spdlog::rotating_logger_mt(
        "default", MIDI_APPLICATION_NAME_SNAKE "_log.txt", max_logfile_size, num_logfiles);
    rotating_logger->sinks().push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
    rotating_logger->flush_on(spdlog::level::err);
    spdlog::set_default_logger(rotating_logger);

    // Setup SDL
    // (Some versions of SDL before <2.0.10 appears to have performance/stalling issues on a
    // minority of Windows systems, depending on whether SDL_INIT_GAMECONTROLLER is enabled or
    // disabled.. updating to latest version of SDL is recommended!)
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
    {
        spdlog::error("Cannot initialize SDL: \"{}\"", SDL_GetError());
        return -1;
    }

    // Setup window
    SDL_WindowFlags window_flags =
        (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_Window* window = SDL_CreateWindow(
        "", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 800, window_flags);

    // Setup SDL_Renderer instance
    SDL_Renderer* renderer =
        SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
    if (renderer == NULL)
    {
        spdlog::error("Cannot create SDL_Renderer: \"{}\"", SDL_GetError());
        return -1;
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImNodes::CreateContext();
    ImGuiIO& io    = ImGui::GetIO();
    io.IniFilename = nullptr;
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    ImNodes::GetIO().EmulateThreeButtonMouse.Modifier = &ImGui::GetIO().KeyCtrl;

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForSDLRenderer(window);
    ImGui_ImplSDLRenderer_Init(renderer);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple
    // fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the
    // font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in
    // your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture
    // when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below
    // will call.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to
    // write a double backslash \\ !
    // io.Fonts->AddFontDefault();
    // io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    // io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    // io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    // io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
    // ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL,
    // io.Fonts->GetGlyphRangesJapanese()); IM_ASSERT(font != NULL);
    // ImGui::GetIO().Fonts->AddFontFromFileTTF("DroidSans.ttf", 16);

    // Main loop
    mc::display::Application app(window);
    bool                     done = false;
    size_t                   frame_idx{};
    while (!done)
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui
        // wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main
        // application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main
        // application. Generally you may always pass all inputs to dear imgui, and hide them from
        // your application based on those two flags.
        mc::KeyboardShortcutAggregator shortcuts;
        SDL_Event                      event;
        while (SDL_PollEvent(&event))
        {
            shortcuts.capture_event(event);
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
            {
                done = true;
            }
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE &&
                event.window.windowID == SDL_GetWindowID(window))
            {
                done = true;
            }
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED &&
                event.window.windowID == SDL_GetWindowID(window))
            {
            }
        }

        app.update_outside_frame();

        // Start the Dear ImGui frame
        ImGui_ImplSDLRenderer_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        app.handle_shortcuts(shortcuts);
        app.render();
        app.handle_done(done);

        // polling is required, imnodes cannot report if something changed
        if (frame_idx++ % 30 == 0)
        {
            SDL_SetWindowTitle(window, app.get_window_title().c_str());
        }

        // Rendering
        ImGui::Render();
        SDL_RenderClear(renderer);
        ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
        SDL_RenderPresent(renderer);
    }

    ImGui_ImplSDLRenderer_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImNodes::DestroyContext();
    ImGui::DestroyContext();

    SDL_DestroyWindow(window);
    SDL_Quit();

    spdlog::default_logger()->flush();

    return 0;
}
