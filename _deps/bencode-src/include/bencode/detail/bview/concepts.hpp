#pragma once

#include <type_traits>
#include <concepts>
#include <nonstd/expected.hpp>

#include "bencode/detail/utils.hpp"
#include "bencode/detail/bad_conversion.hpp"


namespace bencode {

struct bview;
struct integer_bview;
struct string_bview;
struct list_bview;
struct dict_bview;

namespace detail {


// forward declaration
template <serializable T>
constexpr auto convert_from_bview_to(const bview& desc)
        -> nonstd::expected<T, conversion_errc>;
}


template <typename T>
concept bview_alternative_type =
    std::same_as<std::remove_cvref_t<T>, integer_bview> ||
    std::same_as<std::remove_cvref_t<T>, string_bview> ||
    std::same_as<std::remove_cvref_t<T>, list_bview> ||
    std::same_as<std::remove_cvref_t<T>, dict_bview>;


/// The concept convert_from_bview_to is satisfied when the
/// bvalue described by a destriptor can be converted to given
/// type with the get_as<T> accessor function.
template <typename T>
concept retrievable_from_bview =
    requires(bview desc) {
        { detail::convert_from_bview_to<T>(desc) }
            -> std::same_as<nonstd::expected<T, conversion_errc>>;
};

namespace detail {

template <typename T>
concept conversion_from_bview_is_adl_overloaded =
requires (const bview& ref) {
    { bencode_convert_from_bview(customization_for<T>, ref) }
    -> std::same_as<nonstd::expected<T, conversion_errc>>;
};

template<typename T>
concept equality_comparison_with_bview_is_adl_overloaded =
requires(T x, const bencode::bview& bview) {
    { bencode_compare_equality_with_bview(customization_for<T>, bview, x) } -> std::same_as<void>;
};

template<typename T>
concept three_way_comparison_with_bview_is_adl_overloaded =
requires(T x, const bencode::bview& bview) {
    { bencode_compare_three_way_with_bview(customization_for<T>, bview, x) } -> std::same_as<void>;
};



template<typename T>
consteval bool convertible_from_bview_to_tuple_elements_helper()
{
    using IS = std::make_index_sequence<std::tuple_size_v<T>>;
    return std::apply(
            []<std::size_t... Is2>(auto&& ... v) constexpr {
                return (retrievable_from_bview<std::tuple_element_t<Is2, T> > && ...);
            },
            std::forward_as_tuple(IS{})
    );
}

template<typename T>
concept convertible_from_bview_to_tuple_elements = requires() {
    (convertible_from_bview_to_tuple_elements_helper<T>() == true);
};

}

}
