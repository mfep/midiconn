cmake_minimum_required(VERSION 3.22)
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW) # option() honors normal variables

include(FetchContent)
include(deps/CMakeRC.cmake)
set(CMAKE_CXX_STANDARD 20)

project(midiconn VERSION 0.2.1 LANGUAGES CXX)

option(MC_CHECK_FOR_UPDATES "Build ${PROJECT_NAME} to check for updates on start" OFF)

find_package(fmt REQUIRED)
find_package(spdlog REQUIRED)
find_package(SDL2 REQUIRED)
if(WIN32)
    find_package(freetype CONFIG REQUIRED)
else()
    find_package(Freetype REQUIRED)
endif()
if(NOT WIN32 AND MC_CHECK_FOR_UPDATES)
    find_package(CURL REQUIRED)
endif()

set(JSON_BuildTests OFF CACHE INTERNAL "")
set(JSON_MultipleHeaders ON CACHE INTERNAL "")

configure_file(
    inc/Version.hpp.in
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/Version.hpp
)

set(JSON_BuildTests OFF CACHE INTERNAL "")
set(JSON_MultipleHeaders ON CACHE INTERNAL "")
add_subdirectory(deps/json EXCLUDE_FROM_ALL)
set(BUILD_EXAMPLES OFF)
set(SPNG_SHARED OFF)
add_subdirectory(deps/libspng EXCLUDE_FROM_ALL)

set(RTMIDI_API_JACK OFF)
set(RTMIDI_BUILD_STATIC_LIBS ON)
add_subdirectory(deps/rtmidi EXCLUDE_FROM_ALL)

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
    src/Version.cpp
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
    deps/portable-file-dialogs
    deps/IconFontCppHeaders
    "${FREETYPE_INCLUDE_DIRS}")
target_link_libraries(midiconn PRIVATE
    nlohmann_json::nlohmann_json
    rtmidi
    spdlog::spdlog
    spng_static
    SDL2::SDL2
    fmt::fmt
    freetype
    resources)
if(NOT WIN32 AND MC_CHECK_FOR_UPDATES)
    target_link_libraries(midiconn PRIVATE curl)
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
    install(IMPORTED_RUNTIME_ARTIFACTS spdlog::spdlog fmt::fmt SDL2::SDL2 freetype
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")
    target_compile_definitions(midiconn PRIVATE
        WINVER=0x0A00
        NTDDI_VERSION=0x0A000000
    )
    target_compile_options(midiconn PRIVATE "/utf-8")
    target_link_libraries(midiconn PRIVATE
        Shcore.lib
    )
    target_link_options(midiconn PRIVATE "/SUBSYSTEM:WINDOWS")
else()
    set(CPACK_PACKAGE_CONTACT "Lőrinc Serfőző <lorinc.serfozo@mailbox.org>")
    set(CMAKE_PROJECT_HOMEPAGE_URL "https://github.com/mfep/midiconn")
    install(FILES "graphics/mc_logo.svg"
        DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/scalable/apps/"
        RENAME "xyz.safeworlds.midiconn.svg")
    install(FILES "packaging/xyz.safeworlds.midiconn.desktop"
        DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/applications/")
    install(FILES "packaging/xyz.safeworlds.midiconn.metainfo.xml"
        DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/metainfo/")
    install(FILES "packaging/xyz.safeworlds.midiconn.mime.xml"
        DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/mime/packages/")

    # DEB specific
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "libasound2, libfmt8, libfreetype6, libsdl2-2.0-0, libspdlog1")
    if(MC_CHECK_FOR_UPDATES)
        set(CPACK_DEBIAN_PACKAGE_DEPENDS "${CPACK_DEBIAN_PACKAGE_DEPENDS}, libcurl4")
    endif()
    set(CPACK_DEBIAN_PACKAGE_SECTION sound)
endif()

install(TARGETS midiconn
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")
include(CPack)
