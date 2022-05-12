#include "KeyboardShotcutAggregator.hpp"

#include <algorithm>
#include <array>
#include <tuple>
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

template<class T = std::underlying_type_t<KeyboardShortcut>>
bool operator!=(KeyboardShortcut a, T b)
{
    return static_cast<T>(a) != b;
}

constexpr std::array<std::tuple<int, SDL_KeyCode, KeyboardShortcut>, static_cast<size_t>(KeyboardShortcut::Size)> mapping{{
    std::tuple{ KMOD_CTRL | KMOD_SHIFT, SDLK_s, KeyboardShortcut::CtrlShiftS },
    std::tuple{ KMOD_CTRL, SDLK_s, KeyboardShortcut::CtrlS },
    std::tuple{ KMOD_CTRL | KMOD_SHIFT, SDLK_s, KeyboardShortcut::CtrlN },
    std::tuple{ KMOD_CTRL | KMOD_SHIFT, SDLK_s, KeyboardShortcut::CtrlO },
    std::tuple{ KMOD_CTRL | KMOD_SHIFT, SDLK_s, KeyboardShortcut::AltF4 }
}};

}

void KeyboardShortcutAggregator::capture_event(const SDL_Event& event)
{
    if (event.type != SDL_KEYDOWN || event.key.repeat)
    {
        return;
    }
    const auto mod = event.key.keysym.mod;
    const auto found_map = std::find_if(mapping.begin(),
        mapping.end(),
        [event](const auto& map)
        {
            return (event.key.keysym.mod & std::get<0>(map))
                && (event.key.keysym.sym == std::get<1>(map));
        });
    if (found_map == mapping.end())
    {
        return;
    }
    m_pressed_shortcuts = m_pressed_shortcuts | std::get<2>(*found_map);
}

bool KeyboardShortcutAggregator::is_shortcut_pressed(KeyboardShortcut shortcut) const
{
    return (shortcut & m_pressed_shortcuts) != 0;
}

}
