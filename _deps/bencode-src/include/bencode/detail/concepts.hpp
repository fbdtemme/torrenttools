#pragma once
#include <iterator>
#include <ranges>

#include "bencode/detail/utils.hpp"
#include "bencode/detail/bencode_type.hpp"
#include "serialization_traits.hpp"
#include "bencode/bencode_fwd.hpp"

namespace bencode
{

template <typename T>
concept bvalue_or_bview =
    detail::is_instantiation_of_v<basic_bvalue, std::remove_cvref_t<T>> ||
    std::same_as<std::remove_cvref_t<T>, bview> ||
    std::same_as<std::remove_cvref_t<T>, integer_bview> ||
    std::same_as<std::remove_cvref_t<T>, string_bview> ||
    std::same_as<std::remove_cvref_t<T>, list_bview> ||
    std::same_as<std::remove_cvref_t<T>, dict_bview>;
}


namespace bencode::detail {

namespace rng = std::ranges;

/*
The expression std::tuple_size<E>::bvalue must be a well-formed integer constant expression,
and the number of identifiers must equal std::tuple_size<E>::bvalue.

For each identifier, a variable whose type is "reference to std::tuple_element<i, E>::type" is
introduced: lvalue reference if its corresponding initializer is an lvalue, rvalue reference otherwise.
 The initializer for the i-th variable is e.get<i>(), if lookup for the identifier get in the scope
 of E by class member access lookup finds at least one declaration that is a function template
 whose first template parameter is a non-type parameter.
 Otherwise, get<i>(e), where get is looked up by argument-dependent lookup only,
 ignoring non-ADL lookup.
 */

template <std::size_t Idx, typename T>
concept tuple_like_structured_binding_get =
    requires() {
        typename std::tuple_element<Idx, T>::type;
    } &&
    (
    requires (T& t1) {
        { t1.template get<Idx>() }
        -> std::same_as<std::add_lvalue_reference_t<std::tuple_element_t<Idx, T>>>;
    } ||
    requires (T& t1) {
        { get<Idx>(t1) }
        -> std::same_as<std::add_lvalue_reference_t<std::tuple_element_t<Idx, T>>>;
    }
    );


template <typename T, std::size_t... Is>
consteval bool tuple_like_structured_binding_get_impl(std::index_sequence<Is...>)
{ return (tuple_like_structured_binding_get<Is, T> && ...); }

template <typename T>
consteval bool tuple_like_structured_binding_get_helper(T& t)
{
    using IS = std::make_index_sequence<std::tuple_size<T>::value>;
    if constexpr (tuple_like_structured_binding_get_impl(t, IS{}))
        return true;
    static_assert(detail::always_false_v<T>,
            "No member member t.get<T>() or ADL get<T>(t) for type");
}

template <typename T>
concept has_tuple_like_structured_binding =
    requires (T& t) {
        typename std::tuple_size<T>::value_type;
        requires std::integral<typename std::tuple_size<T>::value_type>;
        tuple_like_structured_binding_get_helper(t);
    };


template <typename T, typename... Args>
concept member_assignable_from =
    requires (T x, Args... args) {
        x.assign(std::forward<Args>(args)...);
    };

template <typename T, typename U = rng::range_value_t<T>>
concept supports_back_inserter =
    requires(T t, U value) {
        t.push_back(value);
    };

template <typename T, typename U = rng::range_value_t<T>>
concept supports_front_inserter =
    requires(T t, U value) {
        t.push_front(value);
    };

template <typename T, typename V = rng::range_value_t<T>, typename It = rng::iterator_t<T>>
concept supports_inserter =
    requires(T& container, It iter, V& value) {
        container.insert(iter, value);
    };


template <typename T>
concept key_value_associative_container =
std::constructible_from<T> &&
        std::ranges::input_range<T> && std::ranges::sized_range<T> &&
        requires (T x) {
            typename T::key_type;
            typename T::mapped_type;
        } &&
        std::same_as<std::iter_value_t<T>, std::pair<typename T::key_type const,
                typename T::mapped_type>>;



template <typename T>
concept has_str_member = requires (T x) { x.str(); };

template <typename T>
concept has_string_member = requires (T x) { x.string(); };

template <typename T>
concept has_c_str_member = requires (T x) { { x.c_str() } -> std::same_as<const char*>; };

template <typename T>
concept has_to_string_member = requires (T x) { x.to_string(); };

template <typename T>
concept has_reserve_member = requires (T x, std::size_t n) { x.reserve(n); };


template <typename T>
concept character =
        std::same_as<T, char> ||
        std::same_as<T, wchar_t> ||
        std::same_as<T, char8_t> ||
        std::same_as<T, char16_t> ||
        std::same_as<T, char32_t>;


} // namespace bencode::detail