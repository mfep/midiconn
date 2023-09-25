# midiconn

[![CI badge](https://img.shields.io/github/actions/workflow/status/mfep/midiconn/ci.yml?branch=main)](https://github.com/mfep/midiconn/actions/workflows/ci.yml)
[![Release badge](https://img.shields.io/github/v/release/mfep/midiconn)](https://github.com/mfep/midiconn/releases/latest)
[![License badge](https://img.shields.io/github/license/mfep/midiconn)](./LICENSE.txt)
[![Flathub badge](https://img.shields.io/flathub/downloads/xyz.safeworlds.midiconn?logo=flathub&logoColor=white)](https://flathub.org/apps/details/xyz.safeworlds.midiconn)
![Platforms badge](https://img.shields.io/badge/platform-windows%20%7C%20linux-informational)

**midiconn** is a virtual hub to connect MIDI hardware through the computer.

<img src="./data/graphics/screenshot_0.png" alt="Screenshot of midiconn" width=600px />

## Features
- An intuitive, node-based user interface with predictable interactions - one does not have to be a power user or MIDI expert to use it.
- Detection of device disconnects and reconnects without program restart.
- Channel node to filter and remap MIDI message channels.
- Platform-independent preset files to port the MIDI device setup across various computers.
- Various themes and UI scaling.
- Minimal resource footprint - quick to launch up.

## Installing

### Windows
Download and execute the [installer package of the latest release](https://github.com/mfep/midiconn/releases/latest).

### Linux
The recommended way to install **midiconn** on a Linux system is via the Flathub Flatpak repository.

<a href="https://flathub.org/apps/details/xyz.safeworlds.midiconn" target="_blank"><img alt="Download on Flathub" src="https://flathub.org/assets/badges/flathub-badge-en.png" title="Download on Flathub" width="240"></a>

Alternatively, the build instructions can be found below.

## Building

### Building the Windows installer (x64)
- From the x64 Native Tools Command Prompt.
- Assuming that [CMake](https://cmake.org) and [WiX 3.11](https://wixtoolset.org/) are installed and on the PATH.

```
# Installing prerequisites with vcpkg
> git clone https://github.com/Microsoft/vcpkg.git
> .\vcpkg\bootstrap-vcpkg.bat
> .\vcpkg\vcpkg.exe install spdlog:x64-windows sdl2:x64-windows freetype[core]:x64-windows rtmidi:x64-windows

# Cloning and building midiconn
> git clone --recursive https://github.com/mfep/midiconn.git
> cmake -S .\midiconn -B .\midiconn\build -D CMAKE_TOOLCHAIN_FILE=%cd%\vcpkg\scripts\buildsystems\vcpkg.cmake
> cmake --build .\midiconn\build --config Release
> cd .\midiconn\build
> cpack -C Release
```
### Building the Linux binary package
Make sure that the development packages of the dependencies are installed. The list of library dependencies is the following:
- ALSA
- JSON for Modern C++ ([nlohmann/json](https://github.com/nlohmann/json))
- libfmt
- libfreetype
- libsdl2 (>= 2.0.17)
- libspdlog
- libcurl4 (required only if `MC_CHECK_FOR_UPDATES=ON`)

Additionally, [CMake](https://cmake.org) and a C++20 capable compiler is needed to build **midiconn**.

On Ubuntu (version 22.04 or later), the following script installs these prerequisites:

```shell
$ sudo apt-get update
$ sudo apt-get install -y git build-essential cmake libasound2-dev libfmt-dev libfreetype-dev libsdl2-dev libspdlog-dev nlohmann-json3-dev
```

On Fedora (version 36 or later), the following script installs these prerequisites:

```shell
$ sudo dnf install -y git gcc-c++ cmake alsa-lib-devel fmt-devel freetype-devel json-devel SDL2-devel spdlog-devel
```

Once the prerequisites are installed, **midiconn** can be built and packaged:

```shell
$ git clone --recursive https://github.com/mfep/midiconn.git
$ cmake -S ./midiconn -B ./midiconn/build -D CMAKE_BUILD_TYPE=Release
$ cmake --build ./midiconn/build -j `nproc`
$ cd ./midiconn/build && cpack -C Release
```

### Building the Flatpak

The following script builds and installs the **midiconn** flatpak locally.
It requires the `flatpak-builder` package to be installed on the system.

```shell
$ git clone --recursive https://github.com/mfep/midiconn.git
$ flatpak remote-add --if-not-exists flathub https://flathub.org/repo/flathub.flatpakrepo
$ flatpak install -y flathub org.freedesktop.Sdk//22.08 org.freedesktop.Platform//22.08
$ flatpak-builder --install --user ./midiconn/build/flatpak ./midiconn/data/packaging/xyz.safeworlds.midiconn.yml
```

### CMake configuration options

|Name                   |Description                                       |Default value  |
|-----------------------|--------------------------------------------------|---------------|
|`MC_CHECK_FOR_UPDATES` |Check for available updates on application start. |`OFF`          |
