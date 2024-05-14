cmake_minimum_required(VERSION 3.22)
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW) # option() honors normal variables

include(FetchContent)
include(external/CMakeRC.cmake)

project(midiconn VERSION 0.2.2 LANGUAGES CXX)

option(MC_CHECK_FOR_UPDATES "Build ${PROJECT_NAME} to check for updates on start" OFF)

find_package(fmt REQUIRED)
find_package(spdlog REQUIRED)
find_package(SDL2 REQUIRED)
find_package(nlohmann_json REQUIRED)
if(WIN32)
    find_package(freetype CONFIG REQUIRED)
else()
    find_package(Freetype REQUIRED)
endif()
if(NOT WIN32 AND MC_CHECK_FOR_UPDATES)
    find_package(CURL REQUIRED)
endif()

configure_file(
    src/Version.hpp.in
    ${CMAKE_CURRENT_BINARY_DIR}/src/Version.hpp
)

set(BUILD_EXAMPLES OFF)
set(SPNG_SHARED OFF)
add_subdirectory(external/libspng EXCLUDE_FROM_ALL)

set(RTMIDI_API_JACK OFF)
set(RTMIDI_BUILD_STATIC_LIBS ON)
add_subdirectory(external/rtmidi EXCLUDE_FROM_ALL)
add_subdirectory(src/midi)

set(sources
    ${g_font_cpp_path}
    external/imgui/imgui.cpp
    external/imgui/imgui_demo.cpp
    external/imgui/imgui_draw.cpp
    external/imgui/imgui_tables.cpp
    external/imgui/imgui_widgets.cpp
    external/imgui/backends/imgui_impl_sdl2.cpp
    external/imgui/backends/imgui_impl_sdlrenderer.cpp
    external/imgui/misc/freetype/imgui_freetype.cpp
    external/imnodes/imnodes.cpp
    src/ActivityIndicator.cpp
    src/Application.cpp
    src/ConfigFile.cpp
    src/DisconnectedMidiInNode.cpp
    src/DisconnectedMidiOutNode.cpp
    src/KeyboardShortcutAggregator.cpp
    src/Licenses.cpp
    src/LogNode.cpp
    src/MidiChannelNode.cpp
    src/MidiInNode.cpp
    src/MidiOutNode.cpp
    src/MidiPortWatchdog.cpp
    src/Node.cpp
    src/NodeEditor.cpp
    src/NodeFactory.cpp
    src/NodeSerializer.cpp
    src/main.cpp
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
        data/graphics/appicon.rc)
else()
    list(APPEND sources src/PlatformUtilsLinux.cpp)
endif()
if (MC_CHECK_FOR_UPDATES)
    list(APPEND sources src/UpdateChecker.cpp)
else()
    list(APPEND sources src/UpdateCheckerStub.cpp)
endif()

file(GLOB FONTS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/data/fonts/*")
cmrc_add_resource_library(midiconn_resources "${FONTS}" "${CMAKE_CURRENT_SOURCE_DIR}/data/graphics/mc_logo.png" ALIAS resources WHENCE data)

add_executable(midiconn "${sources}")

target_include_directories(midiconn PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}/src
    external/imgui
    external/imnodes
    external/portable-file-dialogs
    external/IconFontCppHeaders
    "${FREETYPE_INCLUDE_DIRS}")
target_link_libraries(midiconn PRIVATE
    nlohmann_json::nlohmann_json
    rtmidi
    spdlog::spdlog
    spng_static
    SDL2::SDL2
    fmt::fmt
    freetype
    resources
    midi)
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
    set(CPACK_WIX_PRODUCT_ICON "${CMAKE_CURRENT_SOURCE_DIR}/data/graphics/appicon.ico")
    set(CPACK_GENERATOR "WIX")
    set(CPACK_WIX_UPGRADE_GUID "96D23EBE-7D9E-4C53-8CD7-6E5CC32A5CE1")
    set(CPACK_WIX_PATCH_FILE "${CMAKE_CURRENT_SOURCE_DIR}/data/packaging/wix_patch.xml")
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
    install(FILES "data/graphics/mc_logo.svg"
        DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/scalable/apps/"
        RENAME "xyz.safeworlds.midiconn.svg")
    install(FILES "data/packaging/xyz.safeworlds.midiconn.desktop"
        DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/applications/")
    install(FILES "data/packaging/xyz.safeworlds.midiconn.metainfo.xml"
        DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/metainfo/")
    install(FILES "data/packaging/xyz.safeworlds.midiconn.mime.xml"
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
