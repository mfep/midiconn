#pragma once

namespace mc::utils
{

template <class... Args>
struct overloads : Args...
{
    using Args::operator()...;
};

template <class... Ts>
overloads(Ts...) -> overloads<Ts...>;

} // namespace mc::utils
