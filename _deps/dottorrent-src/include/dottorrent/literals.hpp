#pragma once
#include <cstddef>

namespace dottorrent::literals {

constexpr std::size_t operator""_KiB(unsigned long long value) noexcept
{
    return value*1024;
}

constexpr std::size_t operator""_KB(unsigned long long value) noexcept
{
    return value*1000;
}

constexpr std::size_t operator""_MiB(unsigned long long value) noexcept
{
    return value*1024*1024;
}

constexpr std::size_t operator""_MB(unsigned long long value) noexcept
{
    return value*1000*1000;
}

constexpr std::size_t operator""_GiB(unsigned long long value) noexcept
{
    return value*1024*1024*1024;
}

constexpr std::size_t operator""_GB(unsigned long long value) noexcept
{
    return value*1000*1000*1000;
}

constexpr std::size_t operator""_TiB(unsigned long long value) noexcept
{
    return value*1024*1024*1024*1024;
}

constexpr std::size_t operator""_TB(unsigned long long value) noexcept
{
    return value*1000*1000*1000*1000;
}

constexpr std::size_t operator""_bits(unsigned long long value) noexcept
{
    return value / 8;
}

} // namespace dottorrent::literals
