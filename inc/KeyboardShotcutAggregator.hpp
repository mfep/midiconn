#pragma once

union SDL_Event;

namespace mc
{

enum class KeyboardShortcut : unsigned
{
    CtrlN = 1,
    CtrlO = 2,
    CtrlS = 4,
    CtrlShiftS = 8,
    AltF4 = 16,
    Size = 5
};

class KeyboardShortcutAggregator
{
public:
    void capture_event(const SDL_Event &event);
    bool is_shortcut_pressed(KeyboardShortcut shortcut) const;

private:
    KeyboardShortcut m_pressed_shortcuts{};
};

}
