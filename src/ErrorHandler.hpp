#pragma once
#include <exception>
#include <type_traits>
#include <utility>

#include "portable-file-dialogs.h"
#include "spdlog/spdlog.h"

namespace mc
{

template <class Fun, class... Args>
auto wrap_exception(Fun fun, Args&&... args) -> decltype(fun(args...))
{
    try
    {
        return fun(std::forward(args)...);
    }
    catch (std::exception& ex)
    {
        spdlog::error(ex.what());
        pfd::message("Error", ex.what(), pfd::choice::ok, pfd::icon::error);
        if constexpr (!std::is_same_v<decltype(fun(args...)), void>)
            return {};
    }
}

} // namespace mc
