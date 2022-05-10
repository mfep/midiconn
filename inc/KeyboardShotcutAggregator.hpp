#pragma once

union SDL_Event;

namespace mc
{

class KeyboardShortcutAggregator
{
public:
    enum KeyboardShortcut
    {
        CtrlN = 1,
        CtrlO = 2,
        CtrlS = 4,
        CtrlShiftS = 8,
        AltF4 = 16
    };

    void capture_event(const SDL_Event &event);
    bool is_shortcut_pressed(KeyboardShortcut shortcut) const;

private:
    KeyboardShortcut m_pressed_shortcuts{};
};

}
