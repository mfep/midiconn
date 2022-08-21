cmake_minimum_required(VERSION 3.22)

set(CMAKE_CXX_STANDARD 17)

project(midi_connector LANGUAGES CXX)

find_package(fmt REQUIRED)
find_package(spdlog REQUIRED)
find_package(SDL2 REQUIRED)
find_package(Freetype REQUIRED)

configure_file(
    inc/Version.hpp.in
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/Version.hpp
)

set(JSON_BuildTests OFF CACHE INTERNAL "")
set(JSON_MultipleHeaders ON CACHE INTERNAL "")
add_subdirectory(deps/json)
set(RTMIDI_BUILD_TESTING OFF CACHE INTERNAL "")
add_subdirectory(deps/rtmidi)

add_executable(midi_connector
    ${g_font_cpp_path}
    deps/imgui/imgui.cpp
    deps/imgui/imgui_demo.cpp
    deps/imgui/imgui_draw.cpp
    deps/imgui/imgui_tables.cpp
    deps/imgui/imgui_widgets.cpp
    deps/imgui/backends/imgui_impl_sdl.cpp
    deps/imgui/backends/imgui_impl_sdlrenderer.cpp
    deps/imgui/misc/freetype/imgui_freetype.cpp
    deps/imnodes/imnodes.cpp
    src/Application.cpp
    src/ConfigFile.cpp
    src/DisconnectedMidiInNode.cpp
    src/DisconnectedMidiOutNode.cpp
    src/InputObserver.cpp
    src/KeyboardShortcutAggregator.cpp
    src/Licenses.cpp
    src/MidiChannelNode.cpp
    src/MidiEngine.cpp
    src/MidiInNode.cpp
    src/MidiOutNode.cpp
    src/MidiPortWatchdog.cpp
    src/MidiProbe.cpp
    src/Node.cpp
    src/NodeEditor.cpp
    src/NodeFactory.cpp
    src/NodeSerializer.cpp
    src/main.cpp
    src/OutputObserver.cpp
    src/PresetManager.cpp
    src/Theme.cpp
)

target_include_directories(midi_connector PRIVATE
    inc
    deps/imgui
    deps/imnodes
    deps/portable-file-dialogs
    deps/IconFontCppHeaders
    ${FREETYPE_INCLUDE_DIRS})
target_link_libraries(midi_connector PRIVATE
    nlohmann_json::nlohmann_json
    rtmidi
    spdlog::spdlog
    SDL2::SDL2
    fmt::fmt
    ${FREETYPE_LIBRARIES})
target_compile_definitions(midi_connector PRIVATE
    SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_DEBUG
    IMGUI_ENABLE_FREETYPE
)
install(TARGETS midi_connector
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

add_custom_command(TARGET midi_connector POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${CMAKE_CURRENT_LIST_DIR}/fonts/DroidSans.ttf
    ${CMAKE_CURRENT_LIST_DIR}/fonts/forkawesome-webfont.ttf
    $<TARGET_FILE_DIR:midi_connector>
)

if (WIN32)
    target_compile_definitions(midi_connector PRIVATE
        WINVER=0x0A00
        NTDDI_VERSION=0x0A000000
    )
    target_link_libraries(midi_connector PRIVATE
        Shcore.lib
    )
    add_custom_command (TARGET midi_connector POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:rtmidi>
        $<TARGET_FILE:spdlog::spdlog>
        $<TARGET_FILE:fmt::fmt>
        $<TARGET_FILE:SDL2::SDL2>
        $<TARGET_FILE_DIR:midi_connector>
    )
endif()
