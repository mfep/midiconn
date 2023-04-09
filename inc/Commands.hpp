#pragma once

namespace mc
{

template<class T>
concept CommandProcessor = requires(T a)
{
    { a.new_preset() };
    { a.open_preset() };
    { a.save_preset() };
    { a.save_preset_as() };
    { a.exit() };
};

} // namespace mc
