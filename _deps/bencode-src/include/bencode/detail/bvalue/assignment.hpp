#pragma once

#include <bencode/detail/symbol.hpp>
#include <bencode/detail/bvalue/bvalue_policy.hpp>
#include <bencode/detail/bvalue/basic_bvalue.hpp>
#include <bencode/detail/bvalue/concepts.hpp>
#include <bencode/detail/serialization_traits.hpp>
#include <bencode/detail/bvalue/events.hpp>
#include <bencode/detail/events/to_bvalue.hpp>

namespace bencode {

// forward declarations
class bview;

namespace detail {

// forward declaration
template <typename Policy>
constexpr void assign_bview_to_bvalue(customization_point_type<bview>,
                                     basic_bvalue<Policy>&, const bview&);

namespace rng = std::ranges;

template <std::integral T, typename Policy>
constexpr void assign_to_bvalue_integer_impl(customization_point_type<T>,
                                                     basic_bvalue<Policy>& bvalue,
                                                     std::type_identity_t<T> value)
{
    using IntegerType = typename detail::policy_integer_t<Policy>;

    if constexpr (same_size_unsigned_as_bvalue_integer<IntegerType, T> &&
            Policy::enable_unsigned_narrowing_conversion) {
        // throws std::overflow_error on exceptions
        bvalue = numeric_cast<IntegerType>(value);
    } else {
        bvalue = static_cast<IntegerType>(value);
    }
}

// specialization for string literal types

template <typename CharT, typename Policy>
    requires character<CharT> &&
             std::assignable_from<policy_string_t<Policy>, const CharT*>
void assign_to_bvalue_string_impl(customization_point_type<const CharT*>,
                                          basic_bvalue<Policy>& b,
                                          const CharT* value,
                                          priority_tag<4>)
{
    b.emplace_string(value);
}

// specialization for implicit/explicit conversion to string_type.

template <typename T, typename U, typename Policy>
    requires std::same_as<T, std::remove_cvref_t<U>> &&
             std::constructible_from<policy_string_t<Policy>, U>
void assign_to_bvalue_string_impl(customization_point_type<T>,
                                          basic_bvalue<Policy>& b,
                                          U&& value,
                                          priority_tag<3>)
{
    b = policy_string_t<Policy>(std::forward<U>(value));
}

// specialization for implicit/explicit conversion from policy_string_type to std::string_view

template <typename T, typename U, typename CharT, typename Policy>
    requires std::same_as<T, std::remove_cvref_t<U>> &&
             std::convertible_to<U, std::string_view> &&
             std::constructible_from<policy_string_t<Policy>, std::string_view>
void assign_to_bvalue_string_impl(customization_point_type<T>,
                                          basic_bvalue<Policy>& b,
                                          U&& value,
                                          priority_tag<4>)
{
    b.emplace_string(static_cast<std::string_view>(value));
}

// str() or string() member function

template <typename T, typename U, typename Policy>
    requires std::same_as<T, std::remove_cvref_t<U>> &&
            (has_str_member<T> &&
             std::constructible_from<
                    policy_string_t<Policy>, decltype(std::declval<U>().str())>) ||
            (has_string_member<T> &&
              std::constructible_from<
                    policy_string_t<Policy>, decltype(std::declval<U>().string())>)
void assign_to_bvalue_string_impl(customization_point_type<T>,
                                   basic_bvalue<Policy>& b,
                                   U&& value,
                                   priority_tag<2>)
{
    if constexpr ( has_str_member<T> ) {
        b.emplace_string(value.str());
    } else if constexpr ( has_string_member<T> ) {
        b.emplace_string(value.string());
    }
}


// c_str() member function

template <typename T, std::common_with<T> U, typename Policy>
    requires std::same_as<T, std::remove_cvref_t<U>> &&
             std::assignable_from<policy_string_t<Policy>, decltype(std::declval<T>().c_str())> &&
             has_c_str_member<T>
void assign_to_bvalue_string_impl(customization_point_type<T>,
                                   basic_bvalue<Policy>& b,
                                   U&& value,
                                   priority_tag<1>)
{
    b.emplace_string(value.c_str());
}

// character range

template <typename T, typename Policy>
    requires rng::input_range<T> &&
             std::convertible_to<rng::range_reference_t<T>, policy_string_value_t<Policy> > &&
             std::constructible_from<
                     policy_string_t<Policy>, rng::iterator_t<T>, rng::sentinel_t<T> >
void assign_to_bvalue_string_impl(customization_point_type<T>,
                                           basic_bvalue<Policy>& b,
                                           const T& value,
                                           priority_tag<1>)
{
    b.emplace_string(rng::begin(value), rng::end(value));
}

// contiguous std::byte range

template <typename T, typename Policy>
    requires rng::contiguous_range<T> &&
             std::same_as<rng::range_value_t<T>, std::byte>
void assign_to_bvalue_string_impl(customization_point_type<T>,
        basic_bvalue<Policy>& b,
        const T& value,
        priority_tag<0>)
{
    using CharT = policy_string_value_t<Policy>;

    auto& bstring = b.emplace_string();
    if constexpr (detail::has_reserve_member<T>) {
        bstring.reserve(rng::size(value));
    }
    auto v = rng::transform_view(value, [](auto x) { return std::to_integer<CharT>(x); });
    bstring.assign(rng::begin(v), rng::end(v));
}

//===========================================================================//
//  List
//===========================================================================//

template <typename T, typename Policy, typename U>
    requires std::assignable_from<policy_list_t<Policy>, U>
constexpr void assign_to_bvalue_list_impl(
        customization_point_type<T>,
        basic_bvalue<Policy>& bvalue,
        U&& value,
        priority_tag<1>)
{
    bvalue.emplace_list(std::forward<U>(value));
}

template <typename T, typename U, typename Policy>
    requires rng::input_range<T> &&
             assignable_to_bvalue_for<rng::range_value_t<T>, Policy>
constexpr void assign_to_bvalue_list_impl(customization_point_type<T>,
        basic_bvalue<Policy>& b,
        U&& value,
        priority_tag<1>)
{
    using LT = policy_list_t<Policy>;
    auto& blist = b.emplace_list();

    // std::forward_list has no size() member
    if constexpr (rng::sized_range<T> && detail::has_reserve_member<LT>) {
        blist.reserve(rng::size(value));
    }
    // when container is passed as rvalue-ref -> move elements.
    for (auto& v : value)
        blist.push_back(detail::forward_like<U>(v));
}


// Tuple like types, except std::array

template <typename U, typename Policy, typename T>
    requires has_tuple_like_structured_binding<T> &&
             tuple_elements_assignable_to_bvalue<T, Policy>
constexpr void assign_to_bvalue_list_impl(customization_point_type<T>,
                                               basic_bvalue<Policy>& b,
                                               U&& value,
                                               priority_tag<0>)
{
    auto& blist = b.emplace_list();
    std::apply([&](auto&& ... v) {
        (blist.push_back(detail::forward_like<U>(v)), ...);
    }, value);
}

//===========================================================================//
//  Dict
//===========================================================================//

// TODO: split in ordered and unordered containers
template <typename T, typename Policy, typename U>
requires key_value_associative_container<T> &&
        std::constructible_from<policy_string_t<Policy>, typename T::key_type> &&
        assignable_to_bvalue_for<typename T::mapped_type, Policy>
void assign_to_bvalue_dict_impl(customization_point_type<T>,
                                               basic_bvalue<Policy>& b,
                                               U&& value,
                                               priority_tag<0>)
{
    auto& bdict = b.emplace_dict();

    if constexpr (detail::is_instantiation_of_v<std::multimap, T> ||
                  detail::is_instantiation_of_v<std::unordered_multimap, T>) {
        for (auto&& [k, v] : value) {
            if (auto it = bdict.find(k); it != std::end(bdict)) {
                auto&& l = get_list(it->second);
                l.push_back(detail::forward_like<U>(v));
            }
            else {
                auto& l = bdict[detail::forward_like<U>(k)].emplace_list();
                l.push_back(detail::forward_like<U>(v));
            }
        }
    }
    else {
        for (auto&&[k, v] : value) {
            bdict.insert_or_assign(detail::forward_like<U>(k),
                                   detail::forward_like<U>(v));
        }
    }
}

template <typename Policy, typename U, typename T = std::remove_cvref<U>>
void assign_to_bvalue_impl_dispatcher(
        customization_point_type<T>, basic_bvalue<Policy>& bvalue, U&& value)
{
    // Use bencode::serialisation_traits to split the overload set per bencode_type bvalue.
    // This makes build errors easier to debug since we have to check less candidates.
    if constexpr (bencode::serialization_traits<T>::type == bencode_type::integer) {
        assign_to_bvalue_integer_impl(
                customization_for<T>, bvalue, std::forward<U>(value));
    }
    else if constexpr (bencode::serialization_traits<T>::type == bencode_type::string) {
        assign_to_bvalue_string_impl(
                customization_for<T>, bvalue, std::forward<U>(value), priority_tag<5>{});
    }
    else if constexpr (bencode::serialization_traits<T>::type == bencode_type::list) {
        assign_to_bvalue_list_impl(
                customization_for<T>, bvalue, std::forward<U>(value), priority_tag<1>{});
    }
    else if constexpr (bencode::serialization_traits<T>::type == bencode_type::dict) {
        assign_to_bvalue_dict_impl(
                customization_for<T>, bvalue, std::forward<U>(value), priority_tag<5>{});
    }
    else if constexpr (serialization_traits<T>::type == bencode_type::uninitialized && std::same_as<T, bview>) {
        assign_bview_to_bvalue(customization_for<T>, bvalue, std::forward<U>(value));
    }
    else {
        static_assert(detail::always_false<T>::value, "no serializer for T found, check includes!");
    }
}


template <typename U, typename Policy, typename T = std::remove_cvref_t<U>>
void assign_to_bvalue_pointer_impl(
        customization_point_type<T>, basic_bvalue<Policy>& bvalue, U&& value)
{
    using E = typename std::pointer_traits<T>::element_type;

    if constexpr (std::same_as<T, std::weak_ptr<E>>) {
        // weak_ptr cannot be compared to nullptr
        if (value.expired()) {
            assign_to_bvalue(bvalue, E{});
        } else {
            assign_to_bvalue(bvalue, *(value.lock()));
        }
    }
    else {
        if (value == nullptr) {
            assign_to_bvalue(bvalue, E{});
            return;
        }
        assign_to_bvalue(bvalue, *value);
    }
}


template <typename U, typename Policy, typename T = std::remove_cvref_t<U>>
    requires serializable<T>
inline void assign_to_bvalue(basic_bvalue<Policy>& bvalue, U&& value)
{
//    // Check if there is a user-provided specialization found by ADL.
    if constexpr (assignment_to_bvalue_is_adl_overloaded<T, Policy>) {
        bencode_assign_to_bvalue(customization_for<T>, bvalue, std::forward<U>(value));
    }
    // otherwise use event interface to convert to a bvalue temporary.
    if constexpr (event_connecting_is_adl_overloaded<T>) {
        auto consumer = events::to_bvalue<Policy>();
        bencode_connect(customization_for<T>, consumer, value);
        bvalue = std::move(consumer.value());
    }
    else if constexpr (serialization_traits<T>::is_pointer) {
        detail::assign_to_bvalue_pointer_impl(
                customization_for<T>, bvalue, std::forward<U>(value));
    }
    else {
        detail::assign_to_bvalue_impl_dispatcher(
                customization_for<T>, bvalue, std::forward<U>(value));
    }
}

} // namespace bencode::detail
} // namespace bencode
