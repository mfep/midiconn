cmake_minimum_required(VERSION 3.22)
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW) # option() honors normal variables

include(FetchContent)
include(external/CMakeRC.cmake)
include(GNUInstallDirs)

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
find_package(Intl REQUIRED)
if (NOT TARGET Intl::Intl)
    add_library(Intl UNKNOWN IMPORTED)
    target_include_directories(Intl PUBLIC "${Intl_INCLUDE_DIRS}")
    target_link_libraries(Intl PUBLIC "${Intl_LIBRARIES}")
    add_library(Intl::Intl ALIAS Intl)
endif()

set(MC_LOCALE_DIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LOCALEDIR}")
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
    external/imgui/backends/imgui_impl_sdlrenderer2.cpp
    external/imgui/misc/freetype/imgui_freetype.cpp
    external/imnodes/imnodes.cpp
    src/ActivityIndicator.cpp
    src/Application.cpp
    src/ConfigFile.cpp
    src/KeyboardShortcutAggregator.cpp
    src/Licenses.cpp
    src/Locale.cpp
    src/LogNode.cpp
    src/MidiChannelNode.cpp
    src/MidiInNode.cpp
    src/MidiOutNode.cpp
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
    "${FREETYPE_INCLUDE_DIRS}"
)
target_link_libraries(midiconn PRIVATE
    nlohmann_json::nlohmann_json
    rtmidi
    spdlog::spdlog
    spng_static
    SDL2::SDL2
    fmt::fmt
    freetype
    resources
    midi
    Intl::Intl
)
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

find_program(XGETTEXT xgettext)
find_package(Gettext)
if (XGETTEXT AND ${GETTEXT_FOUND})
    set(POT_FILE "${PROJECT_SOURCE_DIR}/po/midiconn.pot")
    add_custom_command(
        OUTPUT "${POT_FILE}"
        COMMAND "${XGETTEXT}" "--add-comments" "--output=${POT_FILE}" "--output-dir=${PROJECT_SOURCE_DIR}/po" ${sources}
        WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
        DEPENDS ${sources}
    )
    add_custom_target(update-pot DEPENDS "${POT_FILE}")

    file(GLOB PO_FILES "${PROJECT_SOURCE_DIR}/po/*.po")
    if(PO_FILES)
        foreach(PO_FILE ${PO_FILES})
            add_custom_command(
                OUTPUT "${PO_FILE}"
                COMMAND "${GETTEXT_MSGMERGE_EXECUTABLE}" "--update" "--backup=off" "${PO_FILE}" "${POT_FILE}"
                WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
                DEPENDS "${POT_FILE}"
            )
        endforeach()

        add_custom_target(update-po DEPENDS ${PO_FILES})

        foreach(PO_FILE ${PO_FILES})
            cmake_path(GET PO_FILE STEM LANG)
            set(MO_FILE "${CMAKE_BINARY_DIR}/${LANG}/midiconn.mo")
            add_custom_command(
                OUTPUT "${MO_FILE}"
                COMMAND "${GETTEXT_MSGFMT_EXECUTABLE}" "--output-file=${MO_FILE}" "${PO_FILE}"
                WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
                DEPENDS "${PO_FILE}"
            )
            add_custom_target(generate-mo-${LANG} ALL DEPENDS ${MO_FILE})
            install(
                FILES "${MO_FILE}"
                DESTINATION "${MC_LOCALE_DIR}/${LANG}/LC_MESSAGES"
            )
        endforeach()
    endif()
endif()
