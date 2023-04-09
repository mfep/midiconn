#pragma once

#include "Commands.hpp"

union SDL_Event;

namespace mc
{

enum class KeyboardShortcut : unsigned
{
    CtrlN      = 1,
    CtrlO      = 2,
    CtrlS      = 4,
    CtrlShiftS = 8,
    Size       = 4
};

class KeyboardShortcutAggregator
{
public:
    void capture_event(const SDL_Event& event);
    bool is_shortcut_pressed(KeyboardShortcut shortcut) const;

    template <CommandProcessor CP>
    void process(CP& command_processor) const
    {
        if (is_shortcut_pressed(KeyboardShortcut::CtrlN))
        {
            command_processor.new_preset();
        }
        if (is_shortcut_pressed(KeyboardShortcut::CtrlO))
        {
            command_processor.open_preset();
        }
        if (is_shortcut_pressed(KeyboardShortcut::CtrlS))
        {
            command_processor.save_preset();
        }
        if (is_shortcut_pressed(KeyboardShortcut::CtrlShiftS))
        {
            command_processor.save_preset_as();
        }
    }

private:
    KeyboardShortcut m_pressed_shortcuts{};
};

} // namespace mc
