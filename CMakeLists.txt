cmake_minimum_required(VERSION 3.22)

include(deps/CMakeRC.cmake)
set(CMAKE_CXX_STANDARD 17)

project(midi_connector VERSION 0.1.0 LANGUAGES CXX)

find_package(fmt REQUIRED)
find_package(spdlog REQUIRED)
find_package(SDL2 REQUIRED)
find_package(rtmidi REQUIRED)
if(WIN32)
    find_package(freetype CONFIG REQUIRED)
else()
    find_package(Freetype REQUIRED)
endif()

configure_file(
    inc/Version.hpp.in
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/Version.hpp
)

set(JSON_BuildTests OFF CACHE INTERNAL "")
set(JSON_MultipleHeaders ON CACHE INTERNAL "")
add_subdirectory(deps/json)

set(sources
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
    src/PlatformUtils.cpp
    src/PresetManager.cpp
    src/Theme.cpp
)

if(WIN32)
    list(APPEND sources
        src/PlatformUtilsWin.cpp
        graphics/appicon.rc)
else()
    list(APPEND sources src/PlatformUtilsLinux.cpp)
endif()

file(GLOB FONTS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/fonts/*")
cmrc_add_resource_library(midi_connector_fonts "${FONTS}" ALIAS fonts)

add_executable(midi_connector "${sources}")

target_include_directories(midi_connector PRIVATE
    inc
    deps/imgui
    deps/imnodes
    deps/portable-file-dialogs
    deps/IconFontCppHeaders
    "${FREETYPE_INCLUDE_DIRS}")
target_link_libraries(midi_connector PRIVATE
    nlohmann_json::nlohmann_json
    RtMidi::rtmidi
    spdlog::spdlog
    SDL2::SDL2
    fmt::fmt
    freetype
    fonts)
target_compile_definitions(midi_connector PRIVATE
    SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_DEBUG
    IMGUI_ENABLE_FREETYPE
)

set(CPACK_PACKAGE_NAME "MIDI Connector")
set(CPACK_PACKAGE_VENDOR "mfep")
if(WIN32)
    set(CMAKE_INSTALL_BINDIR ".")
    set(CPACK_PACKAGE_INSTALL_DIRECTORY "MIDI Connector")
    set(CPACK_PACKAGE_EXECUTABLES midi_connector "MIDI Connector")
    set(CPACK_WIX_PRODUCT_ICON "${CMAKE_CURRENT_SOURCE_DIR}/graphics/appicon.ico")
    set(CPACK_GENERATOR "WIX")
    set(CPACK_WIX_UPGRADE_GUID "96D23EBE-7D9E-4C53-8CD7-6E5CC32A5CE1")
    install(IMPORTED_RUNTIME_ARTIFACTS spdlog::spdlog fmt::fmt SDL2::SDL2 freetype
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")
    target_compile_definitions(midi_connector PRIVATE
        WINVER=0x0A00
        NTDDI_VERSION=0x0A000000
    )
    target_link_libraries(midi_connector PRIVATE
        Shcore.lib
    )
    target_link_options(midi_connector PRIVATE "/SUBSYSTEM:WINDOWS")
else()
    install(FILES "graphics/mc_logo.svg"
        DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/scalable/apps/"
        RENAME "xyz.safeworlds.midi_connector.svg")
    install(FILES "packaging/xyz.safeworlds.midi_connector.desktop"
        DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/applications/")
    install(FILES "packaging/xyz.safeworlds.midi_connector.metainfo.xml"
        DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/metainfo/")
endif()

install(TARGETS midi_connector
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")
include(CPack)
