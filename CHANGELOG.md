# Changelog
All notable changes to this project will be documented in this file. See [conventional commits](https://www.conventionalcommits.org/) for commit guidelines.

- - -
## [v0.3.3](https://github.com/mfep/midiconn/compare/98f5057704bf860cac7b7d45413b32a552b975e3..v0.3.3) - 2025-06-09
#### Bug Fixes
- **(ci)** Remove gettext vcpkg dependency in Windows CI - ([10b03d5](https://github.com/mfep/midiconn/commit/10b03d5dcf0df4fba97fd62d2b3d73b46cce2e20)) - [@mfep](https://github.com/mfep)
- **(i18n)** gettext proxy function - ([cc037dd](https://github.com/mfep/midiconn/commit/cc037dd39a61347ae4c939127475b024f5559bcd)) - [@mfep](https://github.com/mfep)
- **(i18n)** Enable i18n in main based on compilation flag - ([859741e](https://github.com/mfep/midiconn/commit/859741e8b7e93e889e6cb849183061c9ca2167f9)) - [@mfep](https://github.com/mfep)
- **(i18n)** Enable i18n with compilation flag - ([98f5057](https://github.com/mfep/midiconn/commit/98f5057704bf860cac7b7d45413b32a552b975e3)) - [@mfep](https://github.com/mfep)
#### Miscellaneous Chores
- **(docs)** Update release in metainfo - ([783c5d7](https://github.com/mfep/midiconn/commit/783c5d78e3ad9dbcf6a59285945f6e581f50fce0)) - [@mfep](https://github.com/mfep)
- **(docs)** Added donation link - ([1880f3d](https://github.com/mfep/midiconn/commit/1880f3d8d7193f4adbc020805b528b14d42c3e73)) - [@mfep](https://github.com/mfep)

- - -

## [v0.3.2](https://github.com/mfep/midiconn/compare/e63cde7e6317f79df47469138f4a7826f5632b48..v0.3.2) - 2025-05-10
#### Bug Fixes
- **(midi)** Fix channel map node not modifying MIDI data (#1) - ([b5fce47](https://github.com/mfep/midiconn/commit/b5fce47ab524fead659e31106b09e785a64dc109)) - [@mfep](https://github.com/mfep)
- Fix channel node not automatically connecting on mouseup - ([e63cde7](https://github.com/mfep/midiconn/commit/e63cde7e6317f79df47469138f4a7826f5632b48)) - [@mfep](https://github.com/mfep)
#### Miscellaneous Chores
- **(deps)** Update flatpak platform to 24.08 - ([afc0441](https://github.com/mfep/midiconn/commit/afc0441ec6d101553a60d1eeb48c4a7c26209d48)) - [@mfep](https://github.com/mfep)
- **(docs)** Update release notes - ([3c75f81](https://github.com/mfep/midiconn/commit/3c75f811ff79ad643fcd9530cada95ee661fba6d)) - [@mfep](https://github.com/mfep)

- - -

## [v0.3.1](https://github.com/mfep/midiconn/compare/2321bde5a1d3d0aa2c1eee3c41ba3f12d84b35db..v0.3.1) - 2025-01-13
#### Bug Fixes
- **(dependencies)** Install specific versions of fmt and spdlog - ([379e8ad](https://github.com/mfep/midiconn/commit/379e8adb6d8a8cc4e32134e738528f2481f58078)) - [@mfep](https://github.com/mfep)
- **(deps)** Fix spdlog and fmt versions in flatpak manifest - ([60cec71](https://github.com/mfep/midiconn/commit/60cec71ae1205c19668192d9c64091a5e51aa4bb)) - [@mfep](https://github.com/mfep)
- **(deps)** Fix build failure of fmtlib - ([2321bde](https://github.com/mfep/midiconn/commit/2321bde5a1d3d0aa2c1eee3c41ba3f12d84b35db)) - [@mfep](https://github.com/mfep)
- Fix header include problems on Windows - ([718515c](https://github.com/mfep/midiconn/commit/718515c309db085ee89c92d5e455a2bd7fee1d89)) - [@mfep](https://github.com/mfep)
- Fixed HiDPI rendering on Wayland - ([4ea2dd4](https://github.com/mfep/midiconn/commit/4ea2dd416d70d88f667f008e735386b9bb66ed7d)) - [@mfep](https://github.com/mfep)
#### Miscellaneous Chores
- **(docs)** Update metainfo - ([966e74c](https://github.com/mfep/midiconn/commit/966e74c09b4e6bf6af1e23f876b5daa2f55c10fd)) - [@mfep](https://github.com/mfep)

- - -

## [v0.3.0](https://github.com/mfep/midiconn/compare/9e585043acce173f6a1d2ff83de32bd41b46a53a..v0.3.0) - 2024-06-18
#### Bug Fixes
- **(CI)** Fix Windows CI build - ([3076509](https://github.com/mfep/midiconn/commit/30765094001723ef733ccf4665ad21377445a625)) - [@mfep](https://github.com/mfep)
- **(build)** Removed unused m_update_check_finished - ([397f30a](https://github.com/mfep/midiconn/commit/397f30ab5f2a744eb7f370d83b3b7b1c0c8b485f)) - [@mfep](https://github.com/mfep)
- **(docs)** Updated README, extracted BUILD.md - ([3b8636c](https://github.com/mfep/midiconn/commit/3b8636c9c4833b938157f008470710f8b38b50f5)) - [@mfep](https://github.com/mfep)
- **(i18n)** Windows fixes - ([77eaab5](https://github.com/mfep/midiconn/commit/77eaab58913e77dbe8955eb73c25486bbaf5965f)) - [@mfep](https://github.com/mfep)
- **(i18n)** Updated translations in NodeEditor - ([3066004](https://github.com/mfep/midiconn/commit/3066004cf85744a2ec9c2e5420911663f7d9f30b)) - [@mfep](https://github.com/mfep)
- **(midi)** Fix setting message type mask - ([87f1be1](https://github.com/mfep/midiconn/commit/87f1be1d7640ba61c383c78e68a6bcabf0c30e81)) - [@mfep](https://github.com/mfep)
- **(midi)** Fixed disconnect in midi::Node::~Node - ([9921a7e](https://github.com/mfep/midiconn/commit/9921a7e48c205b7af77a0437050b959332582233)) - [@mfep](https://github.com/mfep)
- Fixed incorrect pre-hook in cog.toml - ([0c95cc8](https://github.com/mfep/midiconn/commit/0c95cc8f8e94db9adbd3cdc5e6fae9fdd5cba8f5)) - [@mfep](https://github.com/mfep)
- Load full font glyph range - ([275e11f](https://github.com/mfep/midiconn/commit/275e11f9859a45ce6d20dd8a5621ab463c148622)) - [@mfep](https://github.com/mfep)
#### Features
- **(i18n)** Switching between languages on menu - ([bdf75c2](https://github.com/mfep/midiconn/commit/bdf75c2114e957f6d72b1ef460acff828cfa4443)) - [@mfep](https://github.com/mfep)
- **(i18n)** Hungarian translation - ([bdda17d](https://github.com/mfep/midiconn/commit/bdda17dede9d75e2147b20823dd0bc47f76f82ac)) - [@mfep](https://github.com/mfep)
- **(i18n)** Using gettext in Nodes - ([95a7fc7](https://github.com/mfep/midiconn/commit/95a7fc73067f4e832e95a7e31720eba865df7040)) - [@mfep](https://github.com/mfep)
- **(i18n)** I18N Application.cpp - ([be9e37a](https://github.com/mfep/midiconn/commit/be9e37a570d63ff0e870fd1aaaad4983de1207ad)) - [@mfep](https://github.com/mfep)
- **(midi)** Recognize system messages in Log Node - ([e0e1dc5](https://github.com/mfep/midiconn/commit/e0e1dc5363fd516d8aa5372a6880768e3ea85829)) - [@mfep](https://github.com/mfep)
- **(midi)** Added basic note formatting - ([d980148](https://github.com/mfep/midiconn/commit/d980148b7cf07555674f4e21cb68f4c4f446b655)) - [@mfep](https://github.com/mfep)
- **(midi)** Implemented MIDI message parsing - ([ee07800](https://github.com/mfep/midiconn/commit/ee0780073c8ff9c00fe597ce7aead4d4358aed33)) - [@mfep](https://github.com/mfep)
- **(midi)** Implemented a data oriented model&graph for processing MIDI messages - ([b7a90d8](https://github.com/mfep/midiconn/commit/b7a90d803c0c735c732554c7ba6c18247bd6ff29)) - [@mfep](https://github.com/mfep)
- **(ui)** Setting appropriate size for the Log Node on any scale - ([32c786d](https://github.com/mfep/midiconn/commit/32c786d6a249ab1f2bd48ee06d684d26bfef98c4)) - [@mfep](https://github.com/mfep)
- **(ui)** Add MIDI filtering option per input node - ([2cf21f0](https://github.com/mfep/midiconn/commit/2cf21f046930b31bc4362cb798a0c50ea2e0c340)) - [@mfep](https://github.com/mfep)
- **(ui)** Implemented LogNode - ([d278837](https://github.com/mfep/midiconn/commit/d2788378e6a1a8aa907f3edee516e17c60b70b0c)) - [@mfep](https://github.com/mfep)
- **(ui)** Allow a single instance only for input and output nodes - ([e473193](https://github.com/mfep/midiconn/commit/e4731932f959a4bf241c4aa15c0c7ffe80aebc1f)) - [@mfep](https://github.com/mfep)
- **(ui)** Added MIDI activity indicator to the Channel Node (input + output) - ([d31df33](https://github.com/mfep/midiconn/commit/d31df33f9256e7a14470d4f07d3c307e876ce3ea)) - [@mfep](https://github.com/mfep)
#### Miscellaneous Chores
- **(ci)** Using flatpak-github-actions - ([9b10d3f](https://github.com/mfep/midiconn/commit/9b10d3f7495b34e32a858de9a96fda0c4112f690)) - [@mfep](https://github.com/mfep)
- **(ci)** Restored Ubuntu 22.04 build - ([ce25123](https://github.com/mfep/midiconn/commit/ce2512323a78e09459d7833c8a0f93ee6664924e)) - [@mfep](https://github.com/mfep)
- **(ci)** Using latest image versions - ([1d702c2](https://github.com/mfep/midiconn/commit/1d702c214b54ec98583d45913504c3cd528f6896)) - [@mfep](https://github.com/mfep)
- **(ci)** Updated the release workflow - ([9e58504](https://github.com/mfep/midiconn/commit/9e585043acce173f6a1d2ff83de32bd41b46a53a)) - [@mfep](https://github.com/mfep)
- **(docs)** Updated metainfo - ([515c90a](https://github.com/mfep/midiconn/commit/515c90aa89dcecb7ad7de66bf0840e8d319fe999)) - [@mfep](https://github.com/mfep)
- **(docs)** Updated vcpkg dependencies in the README - ([a6f164e](https://github.com/mfep/midiconn/commit/a6f164e15aa9b17aa0c56c4f43fea57f6a629ed3)) - [@mfep](https://github.com/mfep)
- **(i18n)** Updated POT file - ([7483e2b](https://github.com/mfep/midiconn/commit/7483e2b6469a3f1782b8b9c31f88b5de5111a60a)) - [@mfep](https://github.com/mfep)
- **(i18n)** Localization skeleton - ([eda4c28](https://github.com/mfep/midiconn/commit/eda4c287e920bafd7fd38999b61683b4b72159b4)) - [@mfep](https://github.com/mfep)
- Devel Flatpak spec - ([ea101ae](https://github.com/mfep/midiconn/commit/ea101aeec645fa51c4137bfb47506c022ee41c7f)) - [@mfep](https://github.com/mfep)
- Updated submodule dependencies - ([c036bff](https://github.com/mfep/midiconn/commit/c036bffb871f519bfd5ecd73ed4ec4690d1740bd)) - [@mfep](https://github.com/mfep)
- Updated cog.toml with repo info [skip ci] - ([2d3153a](https://github.com/mfep/midiconn/commit/2d3153a1eefa754e4e27e678dffe88f79fef8248)) - [@mfep](https://github.com/mfep)
- Added cog.toml - ([5cd744c](https://github.com/mfep/midiconn/commit/5cd744c930f6823c7552c8c304e5e2874c4ebdf9)) - [@mfep](https://github.com/mfep)
#### Refactoring
- **(midi)** Moved midi namespace to midi subdir - ([836a2d7](https://github.com/mfep/midiconn/commit/836a2d75da05ab3100ac4130d2e90300fcdfb862)) - [@mfep](https://github.com/mfep)
- Simplified handling of disconnect/reconnect of MIDI devices - ([0be5131](https://github.com/mfep/midiconn/commit/0be5131935db5a6a12126a1d35bad37204312937)) - [@mfep](https://github.com/mfep)

- - -

Changelog generated by [cocogitto](https://github.com/cocogitto/cocogitto).