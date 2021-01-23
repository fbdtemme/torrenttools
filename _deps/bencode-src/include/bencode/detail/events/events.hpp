#pragma once
#include <vector>

#include <bencode/detail/concepts.hpp>

#include <concepts>
#include <type_traits>
#include <memory>

#include "bencode/detail/bencode_type.hpp"
#include "bencode/detail/concepts.hpp"
#include "bencode/detail/events/concepts.hpp"
#include "bencode/detail/serialization_traits.hpp"


// TODO: Modify event calling key does not take a string parameter and must be followed
//         by a call to string. Deduplicates event creating code.

namespace bencode {

namespace rng = std::ranges;

namespace detail {

template<event_consumer EC, typename T>
constexpr void connect_events_integer_impl(
        customization_point_type<T>, EC& consumer, T value, priority_tag<0>)
{
    consumer.integer(static_cast<std::int64_t>(value));
}

// specialization for types

template<typename T, typename U, event_consumer EC>
requires std::same_as<T, std::remove_cvref_t<U>> &&
        std::constructible_from<std::string_view, U>
void connect_events_string_impl(customization_point_type<T>,
        EC& consumer,
        U&& value,
        priority_tag<4>)
{
    consumer.string(forward_like<U>(value));
}

// str() or string() member function

template <typename T, typename U, event_consumer EC>
requires std::same_as<T, std::remove_cvref_t<U>> &&
         (has_str_member<T> &&
                 std::constructible_from<std::string_view, decltype(std::declval<U>().str())>) ||
         (has_string_member<T> &&
                 std::constructible_from<std::string_view, decltype(std::declval<U>().string())>)
void connect_events_string_impl(customization_point_type<T>,
                                  EC& consumer,
                                  U&& value,
                                  priority_tag<3>)
{
    if constexpr (has_str_member<T>) {
        consumer.string(value.str());
    } else if constexpr (has_string_member<T>) {
        consumer.string(value.string());
    }
}




// c_str() member function

template<typename T, typename U, event_consumer EC>
requires std::same_as<T, std::remove_cvref_t<U>> &&
        has_c_str_member<T> &&
        std::constructible_from<std::string_view, decltype(std::declval<U>().c_str())>
void connect_events_string_impl(customization_point_type<T>,
        EC& consumer,
        U&& value,
        priority_tag<2>)
{
    consumer.string(value.c_str());
}


// contiguous character range

template<typename T, event_consumer EC>
requires rng::contiguous_range<T> &&
        std::convertible_to<rng::range_reference_t<T>, typename std::string_view::value_type> &&
        std::constructible_from<std::string_view, rng::iterator_t<T>, rng::sentinel_t<T> >
void connect_events_string_impl(
        customization_point_type<T>, EC& consumer, const T& value, priority_tag<1>)
{
    consumer.string(std::string_view(rng::begin(value), rng::end(value)));
}

// contiguous std::byte range

template<typename T, event_consumer EC>
requires rng::contiguous_range<T> &&
         std::same_as<rng::range_value_t<T>, std::byte>
void connect_events_string_impl(customization_point_type<T>,
        EC& consumer,
        const T& value,
        priority_tag<0>)
{
    consumer.string(std::string_view(
            reinterpret_cast<const char*>(value.data()),
            rng::size(value)));
}


// Tuple like types, except std::array

template<event_consumer EC, typename T, typename U>
    requires has_tuple_like_structured_binding<T> &&
             tuple_elements_are_event_producers<T>
constexpr void connect_events_list_impl(customization_point_type<T>,
        EC& consumer,
        U&& value,
        priority_tag<0>)
{
    consumer.list_begin(std::tuple_size_v<T>);
    std::apply([&](auto&& ... v) {
        (
            (connect(consumer, detail::forward_like<T>(v)),
                     consumer.list_item())
            , ... );
    }, value);
    consumer.list_end(std::tuple_size_v<T>);
}

template<typename T, typename U, event_consumer EC>
requires rng::input_range<T> &&
         event_producer<rng::range_value_t<T>> /*&&
         (!std::same_as<rng::range_value_t<T>, T>)     //  prevent infinite recursion !*/
constexpr void connect_events_list_impl(customization_point_type<T>,
        EC& consumer,
        U&& value,
        priority_tag<1>)
{
    if constexpr (rng::sized_range<T>) {
        consumer.list_begin(std::size(value));
    }
    else {
        consumer.list_begin();
    }
    for (auto&& v : value) {
        connect(consumer, detail::forward_like<U>(v));
        consumer.list_item();
    }

    if constexpr (rng::sized_range<T>) {
        consumer.list_end(std::size(value));
    }
    else {
        consumer.list_end();
    }
}

template<typename T, typename U, event_consumer EC>
    requires rng::input_range<T> &&
            (serialization_traits<typename T::key_type>::type == bencode_type::string) &&
             event_producer<typename T::mapped_type>
constexpr void connect_events_dict_impl(customization_point_type<T>,
        EC& consumer,
        U&& value,
        priority_tag<0>)
{
    using K = typename T::key_type;
    using V = typename T::mapped_type;

    consumer.dict_begin(rng::size(value));

    if constexpr (serialization_traits<T>::key_order == dict_key_order::sorted) {
        for (auto&& [k, v] : value) {
            connect(consumer, detail::forward_like<K>(k));
            consumer.dict_key();

            connect(consumer, detail::forward_like<V>(v));
            consumer.dict_value();
        }
    }
    else {
        // sort keys before serializing

        std::vector<K> keys{};
        keys.reserve(rng::size(value));
        std::transform(rng::begin(value), rng::end(value), std::back_inserter(keys),
                       [](const auto& p) { return p.first; });
        std::sort(keys.begin(), keys.end());

        for (auto&& key : keys) {
            auto it = value.find(key);

            connect(consumer, detail::forward_like<K>(it->first));
            consumer.dict_key();

            connect(consumer, detail::forward_like<V>(it->second));
            consumer.dict_value();
        }
    }
    consumer.dict_end(std::size(value));
}

template <typename T, event_consumer EC>
constexpr void connect_events_pointer_impl(
        customization_point_type<T>, EC& consumer, const T& value)
{
    using E = typename std::pointer_traits<T>::element_type;

    if constexpr (std::same_as<T, std::weak_ptr<E>>) {
    // weak_ptr cannot be compared to nullptr
        if (value.expired()) {
            connect(consumer, E{});
        } else {
            connect(consumer, *value.lock());
        }
    } else {
        if (value == nullptr) {
            connect(consumer, E{});
        }
        else {
            connect(consumer, static_cast<E&>(*value));
        }
    }
}



template<typename T, event_consumer EC>
constexpr void connect_events_runtime_impl(
    customization_point_type<T>, EC& consumer, const T& value, priority_tag<0>)
{
    static_assert(detail::always_false_v<T>, "internal error");
}

template<typename T, event_consumer EC, typename U>
constexpr void connect_events_impl_dispatcher(
        customization_point_type<T>, EC& consumer, U&& producer)
{
    if constexpr (bencode::serialization_traits<T>::type == bencode_type::integer) {
        detail::connect_events_integer_impl(
                customization_for<T>, consumer, std::forward<U>(producer), detail::priority_tag<0>{});
    }
    else if constexpr (bencode::serialization_traits<T>::type == bencode_type::string) {
        detail::connect_events_string_impl(
                customization_for<T>, consumer, std::forward<U>(producer), detail::priority_tag<5>{});
    }
    else if constexpr (bencode::serialization_traits<T>::type == bencode_type::list) {
        detail::connect_events_list_impl(
                customization_for<T>, consumer, std::forward<U>(producer), detail::priority_tag<1>{});
    }
    else if constexpr (bencode::serialization_traits<T>::type == bencode_type::dict) {
        detail::connect_events_dict_impl(
                customization_for<T>, consumer, std::forward<U>(producer), detail::priority_tag<5>{});
    }
    else if constexpr (bencode::serialization_traits<T>::type == bencode_type::uninitialized) {
        detail::connect_events_runtime_impl(
                customization_for<T>, consumer, std::forward<U>(producer), detail::priority_tag<5>{});
    }
    else {
        static_assert(detail::always_false<T>::value, "no serializer for T found, check includes!");
    }
}

} // namespace detail

/// Pass events generated by producer to the consumer.
/// @param producer a value that can generate calls to an event_consumer.
/// @param consumer a value satisfying the event_consumer concept.
template <typename EC, typename U, typename T = std::remove_cvref_t<U>>
/// \cond CONCEPTS
    requires serializable<T> && event_consumer<EC>
/// \endcond
constexpr void connect(EC& consumer, U&& producer)
{
    // Check if there is a user-provided specialization found by ADL.
    if constexpr (detail::event_connecting_is_adl_overloaded<T>) {
        bencode_connect(customization_for<T>, consumer, std::forward<U>(producer));
    }
    else if constexpr (serialization_traits<T>::is_pointer) {
        detail::connect_events_pointer_impl(
                customization_for<T>, consumer, std::forward<U>(producer));
    }
    else {
        detail::connect_events_impl_dispatcher(
                customization_for<T>, consumer, std::forward<U>(producer));
    }
}

} // namespace bencode