#pragma once

#include <cstdint>
#include <cctype>
#include <iosfwd>
#include <string_view>
#include <compare>
#include <array>
#include <gsl-lite/gsl-lite.hpp>



namespace bencode {

/// Enum representing a bencode data type.
enum class bencode_type
{
    uninitialized = 0,
    integer = 1,
    string = 2,
    list = 3,
    dict = 4
};

/// Converts a bencode_data type to a string_view.
/// @param token a bencode_token bvalue
/// @returns A string reprensentation of the bencode_type.
constexpr std::string_view to_string(bencode_type token) noexcept
{
    switch (token) {
    case bencode_type::uninitialized:
        return "uninitialized";
    case bencode_type::integer:
        return "integer";
    case bencode_type::string:
        return "string";
    case bencode_type::list:
        return "list";
    case bencode_type::dict:
        return "dict";
    default:
        return "(unrecognised bencode type)";
    }
}

/// Inserts the string representation of a token into the stream.
/// @params token the bencode_token to print
inline std::ostream& operator<<(std::ostream& os, bencode_type token)
{ return os << to_string(token); }

/// Comparison operator for bencode types
/// This provides a total order relation by assigning an arbitrary order
/// between the different types.
constexpr std::strong_ordering operator<=>(const bencode_type lhs, const bencode_type rhs) noexcept
{
    constexpr std::array<std::uint8_t, 6> order = { 0, 1, 2, 3, 4, };
    const auto l_index = static_cast<std::uint8_t>(lhs);
    const auto r_index = static_cast<std::uint8_t>(rhs);
    Expects(l_index < order.size());
    Expects(r_index < order.size());
    return (order[l_index] <=> order[r_index]);
}


template <bencode_type E>
struct bencode_type_tag
{
    explicit constexpr bencode_type_tag() noexcept = default;

    constexpr operator bencode_type() const
    { return E; }

    constexpr auto index() const { return static_cast<std::uint8_t>(E); }
};

namespace btype {

inline constexpr auto uninitialized = bencode_type_tag<bencode_type::uninitialized>{};
inline constexpr auto integer       = bencode_type_tag<bencode_type::integer>{};
inline constexpr auto string        = bencode_type_tag<bencode_type::string>{};
inline constexpr auto list          = bencode_type_tag<bencode_type::list>{};
inline constexpr auto dict          = bencode_type_tag<bencode_type::dict>{};

}

namespace detail {

template <typename T>
struct is_btype_tag : std::false_type{};

template <bencode_type E>
struct is_btype_tag<bencode_type_tag<E>> : std::true_type {};

}

template <typename T>
concept bvalue_alternative_tag =  detail::is_btype_tag<T>::value;

template <typename T>
concept bview_alternative_tag  =  detail::is_btype_tag<T>::value;


} // namespace bencode