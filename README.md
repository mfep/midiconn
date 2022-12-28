# midiconn

**midiconn** is a virtual hub to connect MIDI hardware through the computer.

*To Do: insert screenshot*

## Features
- An intuitive, node-based user interface with predictable interactions - one does not have to be a power user or MIDI expert to start using it.
- Detection of device disconnects and reconnects without program restart.
- Channel node to filter and remap MIDI message channels.
- Platform-independent preset files to port the MIDI device setup across various computers.
- Various themes and UI scaling.
- Minimal resource footprint - no web technologies are employed in **midiconn**.
- 100% free and open source.

## Installing

## Building

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
Make sure that the development packages of the dependencies are installed or use an image built from the provided [Dockerfile](./Dockerfile). The list of library dependencies is the following:
- libasound2
- libfmt
- libfreetype
- libsdl2 (>= 2.0.17)
- libspdlog

```
$ git clone --recursive https://gitlab.com/mfep/midiconn.git
$ cmake -S ./midiconn -B ./midiconn/build -D CMAKE_BUILD_TYPE=Release
$ cmake --build ./midiconn/build
```
### Linux Flatpak
```
$ git clone --recursive https://gitlab.com/mfep/midiconn.git
$ flatpak remote-add --if-not-exists flathub https://flathub.org/repo/flathub.flatpakrepo
$ flatpak install -y flathub org.freedesktop.Sdk//21.08 org.freedesktop.Platform//21.08
$ flatpak-builder ./midiconn/build/flatpak ./midiconn/packaging/midiconn.yml
```
