cmake_minimum_required(VERSION 3.22)

find_package(rtmidi REQUIRED)

project(midi_connector LANGUAGES CXX)

add_executable(midi_connector
    deps/imgui/imgui.cpp
    deps/imgui/imgui_demo.cpp
    deps/imgui/imgui_draw.cpp
    deps/imgui/imgui_tables.cpp
    deps/imgui/imgui_widgets.cpp
    deps/imgui/backends/imgui_impl_win32.cpp
    deps/imgui/backends/imgui_impl_dx9.cpp
    deps/imnodes/imnodes.cpp
    src/MidiEngine.cpp
    src/MidiNodeObserver.cpp
    src/NodeEditor.cpp
    src/NodeObserver.cpp
    src/main.cpp)

target_include_directories(midi_connector PRIVATE
    inc
    deps/imgui
    deps/imnodes)
target_link_libraries(midi_connector PRIVATE
    D3d9.lib
    Dwmapi.lib
    RtMidi::rtmidi)
