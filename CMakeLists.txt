cmake_minimum_required(VERSION 3.22)

include(FetchContent)
include(deps/CMakeRC.cmake)
set(CMAKE_CXX_STANDARD 17)

project(midiconn VERSION 0.2.0 LANGUAGES C CXX)

option(MC_CHECK_FOR_UPDATES "Build ${PROJECT_NAME} to check for updates on start" OFF)

find_package(fmt REQUIRED)
find_package(spdlog REQUIRED)
find_package(SDL2 REQUIRED)
find_package(rtmidi REQUIRED)
if(WIN32)
    find_package(freetype CONFIG REQUIRED)
else()
    find_package(Freetype REQUIRED)
endif()

set(JSON_BuildTests OFF CACHE INTERNAL "")
set(JSON_MultipleHeaders ON CACHE INTERNAL "")

FetchContent_Declare(cpr
    GIT_REPOSITORY https://github.com/libcpr/cpr.git
    GIT_TAG 1.10.0
    GIT_PROGRESS TRUE
    GIT_SHALLOW TRUE)
if(MC_CHECK_FOR_UPDATES)
    FetchContent_MakeAvailable(cpr)
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
    deps/imgui/backends/imgui_impl_sdl2.cpp
    deps/imgui/backends/imgui_impl_sdlrenderer.cpp
    deps/imgui/misc/freetype/imgui_freetype.cpp
    deps/imnodes/imnodes.cpp
    deps/libspng/spng/spng.c
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
    src/PortNameDisplay.cpp
    src/ResourceLoader.cpp
    src/Theme.cpp
    src/WelcomeWindow.cpp
)

if(WIN32)
    list(APPEND sources
        src/PlatformUtilsWin.cpp
        graphics/appicon.rc)
else()
    list(APPEND sources src/PlatformUtilsLinux.cpp)
endif()
if (MC_CHECK_FOR_UPDATES)
    list(APPEND sources src/UpdateChecker.cpp)
else()
    list(APPEND sources src/UpdateCheckerStub.cpp)
endif()

file(GLOB FONTS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/fonts/*")
cmrc_add_resource_library(midiconn_resources "${FONTS}" "${CMAKE_CURRENT_SOURCE_DIR}/graphics/mc_logo.png" ALIAS resources)

add_executable(midiconn "${sources}")

target_include_directories(midiconn PRIVATE
    inc
    deps/imgui
    deps/imnodes
    deps/libspng/spng
    deps/portable-file-dialogs
    deps/IconFontCppHeaders
    "${FREETYPE_INCLUDE_DIRS}")
target_link_libraries(midiconn PRIVATE
    nlohmann_json::nlohmann_json
    RtMidi::rtmidi
    spdlog::spdlog
    SDL2::SDL2
    fmt::fmt
    freetype
    resources)
if(MC_CHECK_FOR_UPDATES)
    target_link_libraries(midiconn PRIVATE cpr::cpr)
endif()
target_compile_definitions(midiconn PRIVATE
    SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_DEBUG
    IMGUI_ENABLE_FREETYPE
)

set(CPACK_PACKAGE_NAME "midiconn")
set(CPACK_PACKAGE_VENDOR "safeworlds")
if(WIN32)
    set(CMAKE_INSTALL_BINDIR ".")
    set(CPACK_PACKAGE_INSTALL_DIRECTORY "midiconn")
    set(CPACK_PACKAGE_EXECUTABLES midiconn "midiconn")
    set(CPACK_WIX_PRODUCT_ICON "${CMAKE_CURRENT_SOURCE_DIR}/graphics/appicon.ico")
    set(CPACK_GENERATOR "WIX")
    set(CPACK_WIX_UPGRADE_GUID "96D23EBE-7D9E-4C53-8CD7-6E5CC32A5CE1")
    set(CPACK_WIX_PATCH_FILE "${CMAKE_CURRENT_SOURCE_DIR}/packaging/wix_patch.xml")
    set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.txt")
    install(IMPORTED_RUNTIME_ARTIFACTS spdlog::spdlog fmt::fmt SDL2::SDL2 freetype cpr
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")
    target_compile_definitions(midiconn PRIVATE
        WINVER=0x0A00
        NTDDI_VERSION=0x0A000000
    )
    target_link_libraries(midiconn PRIVATE
        Shcore.lib
    )
    target_link_options(midiconn PRIVATE "/SUBSYSTEM:WINDOWS")
else()
    set(CPACK_PACKAGE_CONTACT "Lőrinc Serfőző <lorinc.serfozo@mailbox.org>")
    install(FILES "graphics/mc_logo.svg"
        DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/scalable/apps/"
        RENAME "xyz.safeworlds.midiconn.svg")
    install(FILES "packaging/xyz.safeworlds.midiconn.desktop"
        DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/applications/")
    install(FILES "packaging/xyz.safeworlds.midiconn.metainfo.xml"
        DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/metainfo/")
    install(FILES "packaging/xyz.safeworlds.midiconn.mime.xml"
        DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/mime/packages/")
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "libasound2, libfmt8, libfreetype6, libsdl2-2.0-0, libspdlog1")
    set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "https://gitlab.com/mfep/midiconn")
    set(CPACK_DEBIAN_PACKAGE_SECTION sound)
endif()

install(TARGETS midiconn
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")
include(CPack)
