#pragma once
#include <type_traits>

namespace termcontrol {

#define TERMCONTROL_ENABLE_BITMASK_OPERATORS(x)  \
template<>                           \
struct enum_traits<x> {              \
    static constexpr bool is_bitmask = true;    \
};

template <typename Enum>
struct enum_traits;


template <typename Enum>
concept bitmask_enum = enum_traits<Enum>::is_bitmask;


template <bitmask_enum Enum>
constexpr Enum operator|(Enum lhs, Enum rhs) noexcept
{
    using T = typename std::underlying_type<Enum>::type;
    return static_cast<Enum>(
            static_cast<T>(lhs) | static_cast<T>(rhs));
}

template <bitmask_enum Enum>
constexpr Enum operator&(Enum lhs, Enum rhs) noexcept
{
    using T = typename std::underlying_type<Enum>::type;
    return static_cast<Enum>(
            static_cast<T>(lhs) & static_cast<T>(rhs));
}

template <bitmask_enum Enum>
constexpr Enum operator^(Enum lhs, Enum rhs) noexcept
{
    using T = typename std::underlying_type<Enum>::type;
    return static_cast<Enum>(
            static_cast<T>(lhs) ^ static_cast<T>(rhs));
}

template <bitmask_enum Enum>
constexpr Enum operator~(Enum lhs) noexcept
{
    using T = typename std::underlying_type<Enum>::type;
    return static_cast<Enum>(
            ~static_cast<T>(lhs));
}

template <bitmask_enum Enum>
constexpr Enum& operator|=(Enum& lhs, Enum rhs) noexcept
{
    using T = typename std::underlying_type<Enum>::type;
    lhs = static_cast<Enum>(
            static_cast<T>(lhs) | static_cast<T>(rhs));
    return lhs;
}

template <bitmask_enum Enum>
constexpr Enum& operator&=(Enum& lhs, Enum rhs) noexcept
{
    using T = typename std::underlying_type<Enum>::type;
    lhs = static_cast<Enum>(
            static_cast<T>(lhs) & static_cast<T>(rhs));
    return lhs;
}

template <bitmask_enum Enum>
constexpr Enum& operator^=(Enum& lhs, Enum rhs) noexcept
{
    using T = typename std::underlying_type<Enum>::type;
    lhs = static_cast<Enum>(
            static_cast<T>(lhs) ^ static_cast<T>(rhs));
    return lhs;
}

} // namespace termcontrol