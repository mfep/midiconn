#include "KeyboardShotcutAggregator.hpp"

#include <algorithm>
#include <array>
#include <type_traits>

#include "SDL2/SDL.h"

namespace mc
{
namespace
{

KeyboardShortcut operator|(KeyboardShortcut a, KeyboardShortcut b)
{
    using T = std::underlying_type_t<KeyboardShortcut>;
    return static_cast<KeyboardShortcut>(static_cast<T>(a) | static_cast<T>(b));
}

KeyboardShortcut operator&(KeyboardShortcut a, KeyboardShortcut b)
{
    using T = std::underlying_type_t<KeyboardShortcut>;
    return static_cast<KeyboardShortcut>(static_cast<T>(a) & static_cast<T>(b));
}

template <class T = std::underlying_type_t<KeyboardShortcut>>
bool operator!=(KeyboardShortcut a, T b)
{
    return static_cast<T>(a) != b;
}

struct KeyboardMapEntry
{
    bool             shift_needed;
    bool             ctrl_needed;
    SDL_Keycode      code;
    KeyboardShortcut shortcut;
};

constexpr std::array<KeyboardMapEntry, size_t(KeyboardShortcut::Size)> keyboard_mapping{
    {{true, true, SDLK_s, KeyboardShortcut::CtrlShiftS},
     {false, true, SDLK_s, KeyboardShortcut::CtrlS},
     {false, true, SDLK_n, KeyboardShortcut::CtrlN},
     {false, true, SDLK_o, KeyboardShortcut::CtrlO}}
};

} // namespace

void KeyboardShortcutAggregator::capture_event(const SDL_Event& event)
{
    if (event.type != SDL_KEYDOWN || event.key.repeat)
    {
        return;
    }
    const auto mod           = event.key.keysym.mod;
    const bool shift_pressed = mod & KMOD_SHIFT;
    const bool ctrl_pressed  = mod & KMOD_CTRL;
    const auto found_map =
        std::find_if(keyboard_mapping.begin(), keyboard_mapping.end(), [&](const auto& map) {
            return (shift_pressed == map.shift_needed) && (ctrl_pressed == map.ctrl_needed) &&
                   (event.key.keysym.sym == map.code);
        });
    if (found_map == keyboard_mapping.end())
    {
        return;
    }
    m_pressed_shortcuts = m_pressed_shortcuts | found_map->shortcut;
}

bool KeyboardShortcutAggregator::is_shortcut_pressed(KeyboardShortcut shortcut) const
{
    return (shortcut & m_pressed_shortcuts) != 0;
}

} // namespace mc
