<div align="center">

# midiconn

[![CI badge](https://img.shields.io/github/actions/workflow/status/mfep/midiconn/ci.yml?branch=main)](https://github.com/mfep/midiconn/actions/workflows/ci.yml)
[![Release badge](https://img.shields.io/github/v/release/mfep/midiconn)](https://github.com/mfep/midiconn/releases/latest)
[![License badge](https://img.shields.io/github/license/mfep/midiconn)](./LICENSE.txt)
[![Flathub badge](https://img.shields.io/flathub/downloads/xyz.safeworlds.midiconn?logo=flathub&logoColor=white)](https://flathub.org/apps/details/xyz.safeworlds.midiconn)
![Platforms badge](https://img.shields.io/badge/platform-windows%20%7C%20linux-informational)

**midiconn** is a virtual hub to connect MIDI hardware through the computer.

<img src="./data/graphics/screenshot_0.png" alt="Screenshot of midiconn" width=600px />

</div>

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

Additionally, the following binary packages are provided in the [Releases](https://github.com/mfep/midiconn/releases/latest):

- Ubuntu 22.04 (Debian package)
- Ubuntu 24.04 (Debian package)

## Building

See [BUILD.md](./docs/BUILD.md)
