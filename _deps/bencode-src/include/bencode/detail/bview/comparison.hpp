#pragma once

#include <compare>
#include <gsl-lite/gsl-lite.hpp>



#include "bencode/detail/concepts.hpp"
#include "bencode/detail/bview/concepts.hpp"
#include "bencode/detail/bview/accessors.hpp"


namespace bencode::detail {


//------------------------------------------------------------------------------------------------//
//  Equality comparison
//------------------------------------------------------------------------------------------------//


template <std::integral T>
    requires std::equality_comparable_with<std::int64_t, T>
constexpr auto compare_equality_with_bview_integer_impl(
            customization_point_type<T>,
            const bview& bview,
            T value,
            priority_tag<0>)
{
    if (!holds_integer(bview)) return false;
    return (get_integer(bview) == value);
}

template <typename T>
    requires std::equality_comparable_with<T, std::string_view>
constexpr bool compare_equality_with_bview_string_impl(
        customization_point_type<T>,
        const bview& bview,
        const T& value,
        priority_tag<1>)
{
    if (!holds_string(bview)) return false;
    return (std::string_view(get_string(bview)) == value);
}

template <typename T>
    requires std::equality_comparable_with<rng::range_value_t<T>, string_bview::value_type>
constexpr bool compare_equality_with_bview_string_impl(
        customization_point_type<T>,
        const bview& bview,
        const T& value,
        priority_tag<0>)
{
    if (!holds_string(bview)) return false;
    const auto s = get_string(bview);
    return std::equal(
            rng::begin(s), rng::end(s),
            rng::begin(value), rng::end(value));
}


/// Comparison for byte strings
template <typename T>
    requires std::same_as<std::remove_cv_t<rng::range_value_t<T>>, std::byte>
constexpr bool compare_equality_with_bview_string_impl(
        customization_point_type<T>,
        const bview& bview,
        const T& value,
        priority_tag<0>)
{
    if (!holds_string(bview)) return false;
    const auto& s = get_string(bview);
    return std::equal(
            rng::begin(s), rng::end(s),
            rng::begin(value), rng::end(value),
            [] (char lhs, std::byte rhs) {
                return short(lhs) == to_integer<short>(rhs);
            });
}

template <typename T>
constexpr bool compare_equality_with_bview_list_impl(
        customization_point_type<T>,
        const bview& bv,
        const T& value,
        priority_tag<0>)
{
    if (!holds_list(bv)) return false;
    const auto& l = get_list(bv);
    return std::equal(
            rng::begin(l), rng::end(l),
            rng::begin(value), rng::end(value));
}

template <typename T>
constexpr bool compare_equality_with_bview_dict_impl(
        customization_point_type<T>,
        const bview& bview,
        const T& value,
        priority_tag<0>)
{
    if (!holds_dict(bview)) return false;

    const auto& bdict = get_dict(bview);
    if (rng::size(bdict) != rng::size(value)) return false;

    return std::equal(rng::begin(bdict), rng::end(bdict),
                      rng::begin(value), rng::end(value),
                      [](auto& p1, const auto& p2) {
                          return (p1.first == p2.first) && (p1.second == p2.second);
    }                );
}


//------------------------------------------------------------------------------------------------//
//  Three way comparison
//------------------------------------------------------------------------------------------------//


template <std::integral T>
    requires std::three_way_comparable_with<std::int64_t, T>
constexpr std::weak_ordering compare_three_way_with_bview_integer_impl(
        customization_point_type<T>,
        const bview& bview,
        T value,
        priority_tag<0>)
{
    if (!holds_integer(bview)) return (bview.type() <=> bencode_type::integer);
    return (get_integer(bview) <=> value);
}

template <typename T>
    requires std::three_way_comparable_with<std::string_view, T>
constexpr std::weak_ordering compare_three_way_with_bview_string_impl(
        customization_point_type<T>,
        const bview& bview,
        const T& value,
        priority_tag<1>)
{
    if (!holds_string(bview)) return (bview.type() <=> bencode_type::string);
    return (std::string_view(get_string(bview)) <=> value);
}


template <typename T>
    requires std::three_way_comparable_with<rng::range_value_t<T>, string_bview::value_type>
constexpr std::weak_ordering compare_three_way_with_bview_string_impl(
        customization_point_type<T>,
        const bview& b,
        const T& value,
        priority_tag<0>)
{
    if (!holds_string(b)) return (b.type() <=> bencode_type::string);
    const auto& bstring = get_string(b);

    return std::lexicographical_compare_three_way(
            rng::begin(bstring), rng::end(bstring),
            rng::begin(value), rng::end(value));
}

template <typename T>
    requires std::same_as<std::remove_cv_t<rng::range_value_t<T>>, std::byte>
constexpr std::weak_ordering compare_three_way_with_bview_string_impl(
        customization_point_type<T>,
        const bview& b,
        const T& value,
        priority_tag<0>)
{
    if (!holds_string(b)) return (b.type() <=> bencode_type::string);
    const auto& bstring = get_string(b);

    return std::lexicographical_compare_three_way(
            rng::begin(bstring), rng::end(bstring),
            rng::begin(value), rng::end(value),
            [] (char lhs, std::byte rhs) {
                return short(lhs) <=> to_integer<short>(rhs);
            });
}



template <typename T>
constexpr auto compare_three_way_with_bview_list_impl(
        customization_point_type<T>,
        const bview& b,
        const T& value,
        priority_tag<0>) -> std::weak_ordering
{
    if (!holds_string(b)) return (b.type() <=> bencode_type::string);
    const auto& blist = get_list(b);

    return std::lexicographical_compare_three_way(
            rng::begin(blist), rng::end(blist),
            rng::begin(value), rng::end(value));
}

template <typename T>
constexpr auto compare_three_way_with_bview_dict_impl(
        customization_point_type<T>,
        const bview& b,
        const T& value,
        priority_tag<0>) -> std::weak_ordering
{
    if (!holds_dict(b)) return (b.type() <=> bencode_type::dict);
    const auto& bdict = get_dict(b);

    return std::lexicographical_compare_three_way(
            rng::begin(bdict), rng::end(bdict), rng::begin(value), rng::end(value),
            [](const auto& p1, const auto& p2) -> std::weak_ordering {
                auto cmp = (p1.first <=> p2.first);
                if (cmp == std::weak_ordering::equivalent)
                    return (p1.second <=> p2.second);
                else
                    return cmp;
            });
}

constexpr bool compare_equality_bview_impl(const bview& lhs, const bview& rhs) noexcept
{
    if (lhs.type() == rhs.type()) {
        switch (lhs.type()) {
        case bencode_type::integer:       return get_integer(lhs) == get_integer(rhs);
        case bencode_type::string:        return get_string(lhs)  == get_string(rhs);
        case bencode_type::list:          return get_list(lhs)    == get_list(rhs);
        case bencode_type::dict:          return get_dict(lhs)    == get_dict(rhs);
        case bencode_type::uninitialized: return true;
        }
    }
    return false;
}

constexpr std::weak_ordering compare_three_way_bview_impl(const bview& lhs, const bview& rhs) noexcept
{
    if (lhs.type() == rhs.type()) {
        switch(lhs.type()) {
        case bencode_type::integer:       return get_integer(lhs) <=> get_integer(rhs);
        case bencode_type::string:        return get_string(lhs)  <=> get_string(rhs);
        case bencode_type::list:          return get_list(lhs)    <=> get_list(rhs);
        case bencode_type::dict:          return get_dict(lhs)    <=> get_dict(rhs);
        case bencode_type::uninitialized: return std::weak_ordering::equivalent;
        }
    }
    return lhs.type() <=> rhs.type();
}


template <serializable T>
constexpr bool compare_equality_with_bview(const bview& bv, const T& value)
{
    // Check if there is a user-provided specialization found by ADL.
    if constexpr (equality_comparison_with_bview_is_adl_overloaded<T>) {
        return bencode_compare_equality_with_bview(customization_for<T>, bv, value);
    }
    else {
        // Use bencode::serialisation_traits to split the overload set per bencode_type bview.
        // This makes build errors easier to debug since we have to check less candidates.
        if constexpr (bencode::serialization_traits<T>::type == bencode_type::integer) {
            return compare_equality_with_bview_integer_impl(
                    customization_for<T>, bv, value, priority_tag<0>{});
        }
        else if constexpr (bencode::serialization_traits<T>::type == bencode_type::string) {
            return compare_equality_with_bview_string_impl(
                    customization_for<T>, bv, value, priority_tag<2>{});
        }
        else if constexpr (bencode::serialization_traits<T>::type == bencode_type::list) {
            return compare_equality_with_bview_list_impl(
                    customization_for<T>, bv, value, priority_tag<0>{});
        }
        else if constexpr (bencode::serialization_traits<T>::type == bencode_type::dict) {
            return compare_equality_with_bview_dict_impl(
                    customization_for<T>, bv, value, priority_tag<0>{});
        }
        else {
            static_assert(detail::always_false<T>::value, "no serializer for T found, check includes!");
        }
    }
    return false;
}

template <serializable T>
constexpr std::weak_ordering compare_three_way_with_bview(const bview& bv, const T& value)
{
    // Check if there is a user-provided specialization found by ADL.
    if constexpr (three_way_comparison_with_bview_is_adl_overloaded<T>) {
        return bencode_compare_three_way_with_bview(customization_for<T>, bv, value);
    }
    else {
        // Use bencode::serialisation_traits to split the overload set per bencode_type bview.
        // This makes build errors easier to debug since we have to check less candidates.
        if constexpr (bencode::serialization_traits<T>::type == bencode_type::integer) {
            return compare_three_way_with_bview_integer_impl(
                    customization_for<T>, bv, value, priority_tag<1>{});
        }
        else if constexpr (bencode::serialization_traits<T>::type == bencode_type::string) {
            return compare_three_way_with_bview_string_impl(
                    customization_for<T>, bv, value, priority_tag<1>{});
        }
        else if constexpr (bencode::serialization_traits<T>::type == bencode_type::list) {
            return compare_three_way_with_bview_list_impl(
                    customization_for<T>, bv, value, priority_tag<1>{});
        }
        else if constexpr (bencode::serialization_traits<T>::type == bencode_type::dict) {
            return compare_three_way_with_bview_dict_impl(
                    customization_for<T>, bv, value, priority_tag<1>{});
        }
        else {
            static_assert(detail::always_false<T>::value, "no serializer for T found, check includes!");
        }
    }
    Ensures(false);
}

} // namespace bencode::detail

