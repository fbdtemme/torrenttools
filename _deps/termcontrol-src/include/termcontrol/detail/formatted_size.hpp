#pragma once
#include <cstdint>
#include <limits>
#include <type_traits>
#include <concepts>
#include "concepts.hpp"

namespace termcontrol::detail {

template <typename T>
struct max_formatted_size {};

// digits10 is number of digits representable **without change**. Maximum representation is thus +1.
template <std::unsigned_integral T>
struct max_formatted_size<T> : std::integral_constant<std::size_t, std::numeric_limits<T>::digits10+1> {};

// one extra character for minus sign.
template <std::signed_integral T>
struct max_formatted_size<T> : std::integral_constant<std::size_t, std::numeric_limits<T>::digits10+2> {};

template <typename T>
inline constexpr std::size_t max_formatted_size_v = max_formatted_size<T>::value;

}

namespace termcontrol {
#define TERMCONTROL_DEFINE_MAX_FORMATTED_SIZE(TYPE, VALUE) \
template <> struct detail::max_formatted_size<TYPE> \
        : std::integral_constant<std::size_t, std::size_t{VALUE}> {};
}