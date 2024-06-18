# Building

## Building the Windows installer (x64)
- From the x64 Native Tools Command Prompt.
- Assuming that [CMake](https://cmake.org) and [WiX 3.11](https://wixtoolset.org/) are installed and on the PATH.

```
# Installing prerequisites with vcpkg
> git clone https://github.com/Microsoft/vcpkg.git
> .\vcpkg\bootstrap-vcpkg.bat
> .\vcpkg\vcpkg.exe install nlohmann-json:x64-windows spdlog:x64-windows sdl2:x64-windows freetype[core]:x64-windows zlib:x64-windows gettext:x64-windows

# Cloning and building midiconn
> git clone --recursive https://github.com/mfep/midiconn.git
> cmake -S .\midiconn -B .\midiconn\build -D CMAKE_TOOLCHAIN_FILE=%cd%\vcpkg\scripts\buildsystems\vcpkg.cmake -D X_VCPKG_APPLOCAL_DEPS_INSTALL=ON
> cmake --build .\midiconn\build --config Release
> cd .\midiconn\build
> cpack -C Release
```
## Building the Linux binary package
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

## Building the Flatpak

The following script builds and installs the **midiconn** Flatpak locally.
It requires the `flatpak-builder` package to be installed on the system.

```shell
$ git clone --recursive https://github.com/mfep/midiconn.git
$ flatpak remote-add --if-not-exists flathub https://flathub.org/repo/flathub.flatpakrepo
$ flatpak install -y flathub org.freedesktop.Sdk//23.08 org.freedesktop.Platform//23.08
$ flatpak-builder --install --user ./midiconn/build/flatpak ./midiconn/data/packaging/xyz.safeworlds.midiconn.yml
```

## CMake configuration options

|Name                   |Description                                       |Default value  |
|-----------------------|--------------------------------------------------|---------------|
|`MC_CHECK_FOR_UPDATES` |Check for available updates on application start. |`OFF`          |
