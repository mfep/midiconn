# midiconn

**midiconn** is a virtual hub to connect pieces of MIDI hardware using the computer.

*To Do: insert screenshot*

## Features
- An intuitive, node-based user interface with predictable interactions - one does not have to be a power user or MIDI expert to start using it.
- Platform-independent preset files to port the MIDI device setup across various computers.
- Minimal resource footprint - no web technologies are employed in **midiconn**.
- 100% free and open source.

## Installing

## Build

### Windows installer (x64)
- From the x64 Native Tools Command Prompt.
- Assuming that CMake and WiX 3.11 are installed and on the path.

```
# Installing prerequisites with vcpkg
> git clone https://github.com/Microsoft/vcpkg.git
> .\vcpkg\bootstrap-vcpkg.bat
> .\vcpkg\vcpkg.exe install spdlog:x64-windows sdl2:x64-windows freetype[core]:x64-windows rtmidi:x64-windows
# Cloning and building midiconn
> git clone --recursive https://gitlab.com/mfep/midiconn.git
> cmake -S .\midiconn -B .\midiconn\build -D CMAKE_TOOLCHAIN_FILE=%cd%\vcpkg\scripts\buildsystems\vcpkg.cmake
> cmake --build .\midiconn\build --config Release
> cd .\midiconn\build
> cpack -C Release
```
### Linux binary

### Linux Flatpak
