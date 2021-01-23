//
// Created by fbdtemme on 13/06/19.
//

#pragma once
#include <array>
#include <utility>
#include <numeric>
#include <cstdint>
#include <type_traits>
#include <bit>

#include "bencode/detail/parser/parsing_error.hpp"

#ifdef NDEBUG
#define BENCODE_UNREACHABLE std::is_constant_evaluated() ? __builtin_unreachable() : std::abort();
#else
#define BENCODE_UNREACHABLE __builtin_unreachable()
#endif


namespace bencode {

template <typename T>
struct customization_point_type { using type = T; };

template <typename T>
inline constexpr auto customization_for = customization_point_type<T> {};

}


namespace bencode::detail {


template<size_t I> struct priority_tag : priority_tag<I-1> {};

template<> struct priority_tag<0> {};


template <typename From, typename To>
using apply_value_category_t = std::conditional_t<std::is_lvalue_reference_v<From>,
        std::remove_reference_t<To>&,
        std::remove_reference_t<To>&&>;


template<class T, class U>
decltype(auto) forward_like(U&& u) {
    return std::forward<apply_value_category_t<T, U>>(std::forward<U>(u));
}

/// helper for static_assert

template <typename T>
struct always_false : std::false_type {};

template <typename T>
constexpr bool always_false_v = always_false<T>::value;


template <template<typename...> typename Template, typename T >
struct is_instantiation_of : std::false_type {};


template <template <typename...> typename Template, typename... Args >
struct is_instantiation_of<Template, Template<Args...>> : std::true_type {};

template <template <typename...> typename Template, typename... Args>
constexpr bool is_instantiation_of_v = is_instantiation_of<Template, Args...>::value;


constexpr std::uint32_t base_ten_digits(std::uint32_t x) {
    constexpr  std::array<std::uint8_t,  33> guess {
            0, 0, 0, 0, 1, 1, 1, 2, 2, 2,
            3, 3, 3, 3, 4, 4, 4, 5, 5, 5,
            6, 6, 6, 6, 7, 7, 7, 8, 8, 8,
            9, 9, 9
    };
    constexpr std::array<std::uint32_t,  10> ten_to_the {
            1,
            10,
            100,
            1000,
            10000,
            100000,
            1000000,
            10000000,
            100000000,
            1000000000,
    };
    unsigned int digits = guess[std::bit_width(x)];
    return digits + (x >= ten_to_the[digits]);
}

constexpr std::uint64_t base_ten_digits(std::uint64_t x) {
    constexpr std::array<std::uint8_t,  65> guess {
            0, 0, 0, 0, 1, 1, 1, 2, 2, 2,
            3, 3, 3, 3, 4, 4, 4, 5, 5, 5,
            6, 6, 6, 6, 7, 7, 7, 8, 8, 8,
            9, 9, 9, 9, 10, 10, 10, 11, 11,
            11, 12, 12, 12, 12, 13, 13, 13,
            14, 14, 14, 15, 15, 15, 15, 16,
            16, 16, 17, 17, 17, 18, 18, 18,
            18, 19
    };
    constexpr std::array<std::uint64_t,  19> ten_to_the {
            1,
            10,
            100,
            1000,
            10000,
            100000,
            1000000,
            10000000,
            100000000,
            1000000000,
            10000000000,
            100000000000,
            1000000000000,
            10000000000000,
            100000000000000,
            1000000000000000,
            10000000000000000,
            100000000000000000,
            1000000000000000000,
    };
    unsigned int digits = guess[std::bit_width(x)];
    return digits + (x >= ten_to_the[digits]);
}

constexpr std::uint64_t base_ten_digits(std::int64_t x)
{
    if (x > 0)
        return base_ten_digits(static_cast<std::uint64_t>(x));
    else
        return base_ten_digits(static_cast<std::uint64_t>(-x));
}

constexpr std::int32_t base_ten_digits(std::int32_t x)
{
    if (x > 0)
        return base_ten_digits(static_cast<std::int32_t>(x));
    else
        return base_ten_digits(static_cast<std::int32_t>(-x));
}

static constexpr auto positive_overflow_msg = "numeric_cast: positive overflow";
static constexpr auto negative_overflow_msg = "numeric_cast: negative overflow";

template<typename Dst, typename Src>
inline constexpr auto numeric_cast(Src value) -> Dst
{
    using namespace std::string_view_literals;

    constexpr bool dst_is_signed = std::numeric_limits<Dst>::is_signed;
    constexpr bool src_is_signed = std::numeric_limits<Src>::is_signed;
    constexpr auto dst_max = std::numeric_limits<Dst>::max();
    constexpr auto src_max = std::numeric_limits<Src>::max();
    constexpr auto dst_lowest = std::numeric_limits<Dst>::lowest();
    constexpr auto src_lowest = std::numeric_limits<Src>::lowest();

    constexpr bool may_overflow_pos = dst_max < src_max;
    constexpr bool may_overflow_neg = src_is_signed || (dst_lowest > src_lowest);

    // unsigned <-- unsigned
    if constexpr (!dst_is_signed && !src_is_signed) {
        if (may_overflow_pos && (value > dst_max)) {
            throw std::overflow_error(positive_overflow_msg);
        }
    }
        // unsigned <-- signed
    else if constexpr (!dst_is_signed && src_is_signed) {
        if constexpr (may_overflow_pos && (value > dst_max)) {
            throw std::overflow_error(positive_overflow_msg);
        }
        else if (may_overflow_neg && (value < 0)) {
            throw std::overflow_error(negative_overflow_msg);
        }
    }
        // signed <-- unsigned
    else if constexpr (dst_is_signed && !src_is_signed) {
        if (may_overflow_pos && (value > dst_max)) {
            throw std::overflow_error(positive_overflow_msg);
        }
    }
        // signed <-- signed
    else if constexpr (dst_is_signed && src_is_signed) {
        if (may_overflow_pos && (value > dst_max)) {
            throw std::overflow_error(positive_overflow_msg);
        }
        else if (may_overflow_neg && (value < dst_lowest)) {
            throw std::overflow_error(negative_overflow_msg);
        }
    }

    // limits have been checked, therefore safe to cast
    return static_cast<Dst>(value);
}

} // namespace bencode::detail