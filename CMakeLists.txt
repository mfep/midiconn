cmake_minimum_required(VERSION 3.22)

set(CMAKE_CXX_STANDARD 17)

project(midi_connector LANGUAGES CXX)

find_package(rtmidi REQUIRED)
find_package(spdlog REQUIRED)
find_package(SDL2 REQUIRED)

configure_file(
    inc/Version.hpp.in
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/Version.hpp
)

add_executable(midi_connector
    deps/imgui/imgui.cpp
    deps/imgui/imgui_demo.cpp
    deps/imgui/imgui_draw.cpp
    deps/imgui/imgui_tables.cpp
    deps/imgui/imgui_widgets.cpp
    deps/imgui/backends/imgui_impl_sdl.cpp
    deps/imgui/backends/imgui_impl_sdlrenderer.cpp
    deps/imnodes/imnodes.cpp
    src/Application.cpp
    src/InputObserver.cpp
    src/Licenses.cpp
    src/MidiChannelNode.cpp
    src/MidiEngine.cpp
    src/MidiInNode.cpp
    src/MidiOutNode.cpp
    src/Node.cpp
    src/NodeEditor.cpp
    src/main.cpp
    src/OutputObserver.cpp)

target_include_directories(midi_connector PRIVATE
    inc
    deps/imgui
    deps/imnodes)
target_link_libraries(midi_connector PRIVATE
    RtMidi::rtmidi
    spdlog::spdlog
    SDL2::SDL2)
