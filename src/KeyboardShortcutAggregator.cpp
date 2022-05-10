#include "KeyboardShotcutAggregator.hpp"

#include "SDL2/SDL.h"

namespace mc
{

void KeyboardShortcutAggregator::capture_event(const SDL_Event& event)
{
    if (event.type != SDL_KEYDOWN)
    {
        return;
    }
    const auto mod = event.key.keysym.mod;
    if (mod & (KMOD_CTRL | KMOD_SHIFT))
    {
        if (event.key.keysym.sym == SDLK_z)
        {
        }
    }
}

}
