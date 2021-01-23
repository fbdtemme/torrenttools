#pragma once
//
#include <compare>
#include <concepts>

#include <bencode/detail/utils.hpp>
#include <bencode/detail/bvalue/concepts.hpp>
#include <bencode/detail/concepts.hpp>

//
namespace bencode::detail {

// forward declarations

template <typename Policy, serializable T>
constexpr bool compare_equality_with_bvalue(const basic_bvalue<Policy>& bvalue, const T& value);

template <typename Policy, serializable T>
constexpr std::weak_ordering compare_three_way_with_bvalue(const basic_bvalue<Policy>& bvalue, const T& value);


//------------------------------------------------------------------------------------------------//
//  Equality comparison
//------------------------------------------------------------------------------------------------//


template <std::integral T, typename Policy>
    requires std::equality_comparable_with<policy_integer_t<Policy>, T>
constexpr bool compare_equality_with_bvalue_integer_impl(
            customization_point_type<T>,
            const basic_bvalue<Policy>& bvalue,
            T value,
            priority_tag<0>)
{
    if (!holds_integer(bvalue)) return false;
    return (get_integer(bvalue) == value);
}

template <typename T, typename Policy>
    requires std::equality_comparable_with<policy_string_t<Policy>, T>
constexpr bool compare_equality_with_bvalue_string_impl(
        customization_point_type<T>,
        const basic_bvalue<Policy>& bvalue,
        const T& value,
        priority_tag<3>)
{
    if (!holds_string(bvalue)) return false;
    return (get_string(bvalue) == value);
}

template <typename T, typename Policy>
    requires std::equality_comparable_with<rng::range_value_t<T>, char>
constexpr bool compare_equality_with_bvalue_string_impl(
        customization_point_type<T>,
        const basic_bvalue<Policy>& bvalue,
        const T& value,
        priority_tag<2>)
{
    if (!holds_string(bvalue)) return false;
    const auto& s = get_string(bvalue);
    return std::equal(
            rng::begin(s), rng::end(s),
            rng::begin(value), rng::end(value));
}

/// Comparison for byte strings
template <typename T, typename Policy>
    requires std::same_as<rng::range_value_t<T>, std::byte>
constexpr bool compare_equality_with_bvalue_string_impl(
        customization_point_type<T>,
        const basic_bvalue<Policy>& bvalue,
        const T& value,
        priority_tag<1>)
{
    if (!holds_string(bvalue)) return false;
    const auto& s = get_string(bvalue);
    return std::equal(
            rng::begin(s), rng::end(s),
            rng::begin(value), rng::end(value),
            [] (char lhs, std::byte rhs) {
                return short(lhs) == to_integer<short>(rhs);
            });
}

template <typename T, typename Policy>
    requires detail::has_string_member<T>
constexpr bool compare_equality_with_bvalue_string_impl(
        customization_point_type<T>,
        const basic_bvalue<Policy>& bvalue,
        const T& value,
        priority_tag<0>)
{
    if (!holds_string(bvalue)) return false;
    const auto& s = get_string(bvalue);
    return s == value.string();
}


template <typename T, typename Policy>
    requires std::equality_comparable_with<policy_list_t<Policy>, T>
constexpr bool compare_equality_with_bvalue_list_impl(
        customization_point_type<T>,
        const basic_bvalue<Policy>& bvalue,
        const T& value,
        priority_tag<0>)
{
    if (!holds_list(bvalue)) return false;
    return (get_list(bvalue) == value);
}


template <typename T, typename Policy>
    requires std::equality_comparable_with<policy_dict_t<Policy>, T>
constexpr auto compare_equality_with_bvalue_dict_impl(
        customization_point_type<T>,
        const basic_bvalue<Policy>& bvalue,
        const T& value,
        priority_tag<1>) -> bool
{
    if (!holds_dict(bvalue)) return false;
    return (get_dict(bvalue) == value);
}


template <typename T, typename Policy>
constexpr bool compare_equality_with_bvalue_dict_impl(
        customization_point_type<T>,
        const basic_bvalue<Policy>& bvalue,
        const T& value,
        priority_tag<0>)
{
    if (!holds_dict(bvalue)) return false;

    const auto& bdict = get_dict(bvalue);
    if (rng::size(bdict) != rng::size(value)) return false;

    return std::equal(rng::begin(bdict), rng::end(bdict),
                      rng::begin(value), rng::end(value),
                      [](const auto& p1, const auto& p2) {
                          return (p1.first == p2.first) && (p1.second == p2.second);
    }                );
}


//------------------------------------------------------------------------------------------------//
//  Three way comparison
//------------------------------------------------------------------------------------------------//


template <std::integral T, typename Policy>
    requires std::three_way_comparable_with<policy_integer_t<Policy>, T>
constexpr std::weak_ordering compare_three_way_with_bvalue_integer_impl(
        customization_point_type<T>,
        const basic_bvalue<Policy>& bvalue,
        T value,
        priority_tag<0>)
{
    if (!holds_integer(bvalue)) return (bvalue.type() <=> bencode_type::integer);
    return (get_integer(bvalue) <=> value);
}

template <typename T, typename Policy>
    requires std::three_way_comparable_with<policy_string_t<Policy>, T>
constexpr std::weak_ordering
compare_three_way_with_bvalue_string_impl(
        customization_point_type<T>,
        const basic_bvalue<Policy>& bvalue,
        const T& value,
        priority_tag<2>)
{
    if (!holds_string(bvalue)) return (bvalue.type() <=> bencode_type::string);
    return (get_string(bvalue) <=> value);
}


template <typename T, typename Policy>
    requires std::three_way_comparable_with<rng::range_value_t<T>, char>
constexpr std::weak_ordering
compare_three_way_with_bvalue_string_impl(
        customization_point_type<T>,
        const basic_bvalue<Policy>& b,
        const T& value,
        priority_tag<1>)
{
    if (!holds_string(b)) return (b.type() <=> bencode_type::string);
    const auto& bstring = get_string(b);

    return std::lexicographical_compare_three_way(
            rng::begin(bstring), rng::end(bstring),
            rng::begin(value), rng::end(value));
}



/// Comparison for byte strings
template <typename T, typename Policy>
    requires std::same_as<rng::range_value_t<T>, std::byte>
constexpr std::weak_ordering
compare_three_way_with_bvalue_string_impl(
        customization_point_type<T>,
        const basic_bvalue<Policy>& bvalue,
        const T& value,
        priority_tag<0>)
{
    if (!holds_string(bvalue)) return (bvalue.type() <=> bencode_type::string);
    const auto& bstring = get_string(bvalue);
    return std::lexicographical_compare_three_way(
            rng::begin(bstring), rng::end(bstring),
            rng::begin(value), rng::end(value),
            [] (char lhs, std::byte rhs) {
                return short(lhs) <=> to_integer<short>(rhs);
            });
}

template <typename T, typename Policy>
    requires detail::has_string_member<T>
constexpr std::weak_ordering
compare_three_way_with_bvalue_string_impl(
        customization_point_type<T>,
        const basic_bvalue<Policy>& bvalue,
        const T& value,
        priority_tag<0>)
{
    if (!holds_string(bvalue)) return (bvalue.type() <=> bencode_type::string);
    const auto& s = get_string(bvalue);
    return s <=> value.string();
}


template <typename T, typename Policy>
    requires std::three_way_comparable_with<policy_list_t<Policy>, T>
constexpr auto compare_three_way_with_bvalue_list_impl(
        customization_point_type<T>,
        const basic_bvalue<Policy>& bvalue,
        const T& value,
        priority_tag<1>) -> std::weak_ordering
{
    if (!holds_list(bvalue)) return (bvalue.type() <=> bencode_type::list);
    return (get_list(bvalue) <=> value);
}


template <typename T, typename Policy>
constexpr std::weak_ordering
compare_three_way_with_bvalue_list_impl(
        customization_point_type<T>,
        const basic_bvalue<Policy>& b,
        const T& value,
        priority_tag<0>)
{
    if (!holds_string(b)) return (b.type() <=> bencode_type::string);
    const auto& blist = get_list(b);

    return std::lexicographical_compare_three_way(
            rng::begin(blist), rng::end(blist),
            rng::begin(value), rng::end(value));
}


template <typename T, typename Policy>
    requires std::three_way_comparable_with<policy_dict_t<Policy>, T>
constexpr auto compare_three_way_with_bvalue_dict_impl(
        customization_point_type<T>,
        const basic_bvalue<Policy>& bvalue,
        const T& value,
        priority_tag<1>) -> std::weak_ordering
{
    if (!holds_dict(bvalue)) return (bvalue.type() <=> bencode_type::dict);
    return (get_dict(bvalue) <=> value);
}

template <typename T, typename Policy>
requires std::three_way_comparable_with<policy_dict_t<Policy>, T>
constexpr std::weak_ordering
compare_three_way_with_bvalue_dict_impl(
        customization_point_type<T>,
        const basic_bvalue<Policy>& b,
        const T& value,
        priority_tag<0>)
{
    if (!holds_dict(b)) return (b.type() <=> bencode_type::dict);
    const auto& bdict = get_dict(b);

    return std::lexicographical_compare_three_way(
            rng::begin(bdict), rng::end(bdict), rng::begin(value), rng::end(value),
            [](const auto& p1, const auto& p2) {
                return (p1.first==p2.first) && (p1.second==p2.second);
            });
}


template <typename Policy, typename T>
inline bool compare_equality_with_bvalue_pointer_impl(
        customization_point_type<T>, const basic_bvalue<Policy>& bvalue, const T& value)
{
    using E = typename std::pointer_traits<T>::element_type;

    if constexpr (std::same_as<T, std::weak_ptr<E>>) {
        // weak_ptr cannot be compared to nullptr
        if (value.expired()) {
            return false;
        } else {
            return compare_equality_with_bvalue(bvalue, *(value.lock()));
        }
    }
    else {
        if (value == nullptr) {
            return false;
        }
        return compare_equality_with_bvalue(bvalue, *value);
    }
}

template <typename Policy, typename T>
inline std::weak_ordering compare_three_way_with_bvalue_pointer_impl(
        customization_point_type<T>, const basic_bvalue<Policy>& bvalue, const T& value)
{
    using E = typename std::pointer_traits<T>::element_type;

    if constexpr (std::same_as<T, std::weak_ptr<E>>) {
        // weak_ptr cannot be compared to nullptr
        if (value.expired()) {
            return bvalue.type() <=> serialization_traits<E>::type;
        } else {
            return compare_three_way_with_bvalue(bvalue, *(value.lock()));
        }
    }
    else {
        if (value == nullptr) {
            return bvalue.type() <=> serialization_traits<E>::type;
        }
        return compare_three_way_with_bvalue(bvalue, *value);
    }
}



template <typename T, typename Policy>
constexpr bool compare_equality_with_bvalue_impl_dispatcher(
        customization_point_type<T>,
        const basic_bvalue<Policy>& bvalue,
        const T& value)
{
//    Use bencode::serialisation_traits to split the overload set per bencode_type bvalue.
    // This makes build errors easier to debug since we have to check less candidates.
    if constexpr (bencode::serialization_traits<T>::type == bencode_type::integer) {
        return compare_equality_with_bvalue_integer_impl(
                customization_for<T>, bvalue, value, priority_tag<0>{});
    }
    else if constexpr (bencode::serialization_traits<T>::type == bencode_type::string) {
        return compare_equality_with_bvalue_string_impl(
                customization_for<T>, bvalue,value, priority_tag<4>{});
    }
    else if constexpr (bencode::serialization_traits<T>::type == bencode_type::list) {
        return compare_equality_with_bvalue_list_impl(
                customization_for<T>, bvalue,value, priority_tag<0>{});
    }
    else if constexpr (bencode::serialization_traits<T>::type == bencode_type::dict) {
        return compare_equality_with_bvalue_dict_impl(
                customization_for<T>, bvalue,value, priority_tag<0>{});
    }
    else {
        static_assert(detail::always_false<T>::value, "no serializer for T found, check includes!");
    }
}

template <typename T, typename Policy>
constexpr std::weak_ordering compare_three_way_with_bvalue_impl_dispatcher(
        customization_point_type<T>,
        const basic_bvalue<Policy>& bvalue,
        const T& value)
{
//    Use bencode::serialisation_traits to split the overload set per bencode_type bvalue.
    // This makes build errors easier to debug since we have to check less candidates.
    if constexpr (bencode::serialization_traits<T>::type == bencode_type::integer) {
        return compare_three_way_with_bvalue_integer_impl(
                customization_for<T>, bvalue, value, priority_tag<0>{});
    }
    else if constexpr (bencode::serialization_traits<T>::type == bencode_type::string) {
        return compare_three_way_with_bvalue_string_impl(
                customization_for<T>, bvalue,value, priority_tag<4>{});
    }
    else if constexpr (bencode::serialization_traits<T>::type == bencode_type::list) {
        return compare_three_way_with_bvalue_list_impl(
                customization_for<T>, bvalue,value, priority_tag<0>{});
    }
    else if constexpr (bencode::serialization_traits<T>::type == bencode_type::dict) {
        return compare_three_way_with_bvalue_dict_impl(
                customization_for<T>, bvalue,value, priority_tag<0>{});
    }
    else {
        static_assert(detail::always_false<T>::value, "no serializer for T found, check includes!");
    }
}


template <typename Policy, serializable T>
constexpr bool compare_equality_with_bvalue(const basic_bvalue<Policy>& bvalue, const T& value)
{
    // Check if there is a user-provided specialization found by ADL.
    if constexpr (equality_comparison_with_bvalue_is_adl_overloaded<T, Policy>) {
        return bencode_compare_equality_with_bvalue(customization_for<T>, bvalue, value);
    }
    else if constexpr (serialization_traits<T>::is_pointer) {
        return compare_equality_with_bvalue_pointer_impl(customization_for<T>, bvalue, value);
    }
    else {
        return compare_equality_with_bvalue_impl_dispatcher(customization_for<T>, bvalue, value);
    }
    Ensures(false);
}

template <typename Policy, serializable T>
constexpr std::weak_ordering compare_three_way_with_bvalue(const basic_bvalue<Policy>& bvalue, const T& value)
{
    // Check if there is a user-provided specialization found by ADL.
    if constexpr (three_way_comparison_with_bvalue_is_adl_overloaded<T, Policy>) {
        return bencode_compare_three_way_with_bvalue(customization_for<T>, bvalue, value);
    }
    else if constexpr (serialization_traits<T>::is_pointer) {
        return compare_three_way_with_bvalue_pointer_impl(customization_for<T>, bvalue, value);
    }
    else {
        return compare_three_way_with_bvalue_impl_dispatcher(customization_for<T>, bvalue, value);
    }
    Ensures(false);
}




} // namespace bencode::detail