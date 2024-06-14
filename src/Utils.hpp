#pragma once

#include <filesystem>
#include <string>
#include <utility>
#include <variant>

namespace mc::utils
{

template <class... Args>
struct overloads : Args...
{
    using Args::operator()...;
};

template <class... Ts>
overloads(Ts...) -> overloads<Ts...>;

// from https://github.com/microsoft/GSL/blob/main/include/gsl/util
// License: MIT
// final_action allows you to ensure something gets run at the end of a scope
template <class F>
class final_action
{
public:
    explicit final_action(const F& ff) noexcept : f{ff} {}
    explicit final_action(F&& ff) noexcept : f{std::move(ff)} {}

    ~final_action() noexcept
    {
        if (invoke)
            f();
    }

    final_action(final_action&& other) noexcept
        : f(std::move(other.f)), invoke(std::exchange(other.invoke, false))
    {
    }

    final_action(const final_action&)   = delete;
    void operator=(const final_action&) = delete;
    void operator=(final_action&&)      = delete;

private:
    F    f;
    bool invoke = true;
};

inline std::string path_to_utf8str(const std::filesystem::path& path)
{
    const std::u8string u8str = path.generic_u8string();
    return {u8str.begin(), u8str.end()};
}

template <class... Args>
struct variant_cast_proxy
{
    std::variant<Args...> v;

    template <class... ToArgs>
    operator std::variant<ToArgs...>() const
    {
        return std::visit(
            [](auto&& arg) -> std::variant<ToArgs...> {
                return arg;
            },
            v);
    }
};

template <class... Args>
auto variant_cast(const std::variant<Args...>& v) -> variant_cast_proxy<Args...>
{
    return {v};
}

} // namespace mc::utils
