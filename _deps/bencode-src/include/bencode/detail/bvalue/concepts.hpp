#pragma once

#include <concepts>
#include <ranges>
#include <type_traits>

#include <nonstd/expected.hpp>
#include <bencode/detail/bad_conversion.hpp>

#include "bencode/bencode_fwd.hpp"
#include "bencode/detail/utils.hpp"

namespace bencode {

/// The concept basic_bvalue_instantiation<T> is satisified if T is a template instantiation of
/// the basic_bvalue template class.
/// Ignores cv-qualifiers and reference types.
template <typename T>
concept basic_bvalue_instantiation =
    detail::is_instantiation_of_v<basic_bvalue, std::remove_cvref_t<T> >;


/// The concept bvalue_alternative_for<T, BV> specifies that T is one of the types
/// that can be stored in the basic_bvalue variant type BV.
/// Ignores cv-qualifiers and reference types.
template <typename T, typename BV>
concept bvalue_alternative_type =
    basic_bvalue_instantiation<BV> && (
        std::same_as<std::remove_cvref_t<T>, typename std::remove_cvref_t<BV>::uninitialized_type> ||
        std::same_as<std::remove_cvref_t<T>, typename std::remove_cvref_t<BV>::integer_type> ||
        std::same_as<std::remove_cvref_t<T>, typename std::remove_cvref_t<BV>::string_type> ||
        std::same_as<std::remove_cvref_t<T>, typename std::remove_cvref_t<BV>::list_type> ||
        std::same_as<std::remove_cvref_t<T>, typename std::remove_cvref_t<BV>::dict_type>
);



namespace detail {

// forward declaration.
template <typename U, typename Policy, typename T = std::remove_cvref_t<U>>
    requires serializable<T>
inline void assign_to_bvalue(basic_bvalue<Policy>& bvalue, U&& value);


// forward declaration.
template <serializable T, basic_bvalue_instantiation U>
constexpr nonstd::expected<T, conversion_errc> convert_from_bvalue_to(U&& bvalue) noexcept;


}


/// Check if a type can be assigned to a basic_bvalue<Policy> type using built-in conversions or
/// by using the user-defined extension point function `bencode_assign_to_value` for given T.
template <typename T, typename Policy>
concept assignable_to_bvalue_for =
    serializable<T> &&
    requires(basic_bvalue<Policy> b, T x) {
        detail::assign_to_bvalue(b, x);
    };


/// Check if a basic_bvalue<Policy> can be converted to type T using built-in conversions or
/// the user defined extension point function 'bencode_convert_from_bvalue.
template <typename T, typename Policy>
concept retrievable_from_bvalue_for =
    serializable<T> &&
    requires(basic_bvalue<Policy> b) {
        { detail::convert_from_bvalue_to<T>(b) } -> std::same_as<nonstd::expected<T, conversion_errc> >;
    };



namespace detail {

template<typename T, typename Policy>
concept assignment_to_bvalue_is_adl_overloaded =
    requires(T x, basic_bvalue<Policy> bvalue) {
        { bencode_assign_to_bvalue(customization_for<T>, bvalue, x) } -> std::same_as<void>;
};


template<typename T, typename Policy>
concept conversion_from_bvalue_is_adl_overloaded =
    requires(T x, bencode::basic_bvalue<Policy> bvalue) {
        { bencode_convert_from_bvalue(customization_for<T>, bvalue, x) } -> std::same_as<void>;
    };

template<typename T, typename Policy>
concept equality_comparison_with_bvalue_is_adl_overloaded =
requires(T x, bencode::basic_bvalue<Policy> bvalue) {
    { bencode_compare_equality_with_bvalue(customization_for<T>, bvalue, x) } -> std::same_as<void>;
};

template<typename T, typename Policy>
concept three_way_comparison_with_bvalue_is_adl_overloaded =
    requires(T x, bencode::basic_bvalue<Policy> bvalue) {
        { bencode_compare_three_way_with_bvalue(customization_for<T>, bvalue, x) } -> std::same_as<void>;
    };


template <typename SignedType, typename UnsignedType>
concept same_size_unsigned_as_bvalue_integer =
    std::signed_integral<SignedType> &&
        std::unsigned_integral<UnsignedType> &&
        (sizeof(SignedType) == sizeof(UnsignedType));

template <typename T, typename Policy>
consteval bool tuple_element_assignable_to_bvalue_helper()
{
    using IS = std::make_index_sequence<std::tuple_size<T>::value>;
    auto r = std::apply([]<std::size_t... Is2>(auto&&... v) constexpr {
        return (assignable_to_bvalue_for<std::tuple_element_t<Is2, T>, Policy> && ...);
    }, std::forward_as_tuple(IS{}));

    if constexpr (r) return true;
    static_assert(detail::always_false_v<T>,
            "No member member t.get<T>() or ADL get<T>(t) for type");
}

template <typename T, typename Policy>
consteval bool convertible_from_bvalue_to_tuple_elements_helper()
{
    using IS = std::make_index_sequence<std::tuple_size<T>::value>;
    auto r = std::apply(
        []<std::size_t... Is2>(auto&&... v) constexpr {
            return (retrievable_from_bvalue_for<std::tuple_element_t<Is2, T>, Policy> && ...);
        },
        std::forward_as_tuple(IS{})
    );

    if constexpr (r) { return true; }
    static_assert(detail::always_false_v<T>, "No conversion defined for all elements.");
}

template <typename T, typename Policy>
concept tuple_elements_assignable_to_bvalue =
    requires (T& t) {
        tuple_element_assignable_to_bvalue_helper<T, Policy>();
    };


template <typename T, typename Policy>
concept convertible_from_bvalue_to_tuple_elements =
requires (T t) {
    convertible_from_bvalue_to_tuple_elements_helper<T, Policy>();
};


} // namespace detail
} // namespacee bencode