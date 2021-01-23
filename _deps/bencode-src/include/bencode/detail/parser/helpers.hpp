#pragma once
#include <cstdint>
#include <type_traits>
#include <concepts>

namespace bencode::detail
{
template <std::integral T>
constexpr bool raise_and_add_safe(T& value, std::type_identity_t<T> base, std::type_identity_t<T> c)
{
    if (__builtin_mul_overflow(value, base, &value) ||
        __builtin_add_overflow(value, c, &value))
        return false;
    return true;
}

template <std::integral T>
constexpr void raise_and_add(T& value1, std::type_identity_t<T> base, std::type_identity_t<T> value2)
{
    value1 = value1 * base + value2;
}
}