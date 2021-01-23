#pragma once

#include <nonstd/expected.hpp>
#include <map>

#include "bencode/detail/symbol.hpp"
#include "bencode/detail/utils.hpp"
#include "bencode/detail/concepts.hpp"
#include "bencode/detail/bad_conversion.hpp"
#include "bencode/detail/bview/concepts.hpp"
#include "bencode/detail/bview/bview.hpp"
#include "bencode/detail/bview/accessors.hpp"



namespace bencode::detail {

namespace rng = std::ranges;

template <typename T>
constexpr nonstd::expected<T, conversion_errc>
convert_from_bview_default_integer_impl(customization_point_type<T>, const bview& v) noexcept
{
    if (!holds_integer(v)) [[unlikely]]
        return nonstd::make_unexpected(conversion_errc::not_integer_type);
    return static_cast<T>(get_integer(v));
}


// Conversion via iterators pair constructor

template <typename T>
    requires std::same_as<rng::range_value_t<T>, char> &&
             std::constructible_from<T, const char*, const char*>
constexpr nonstd::expected<T, conversion_errc>
convert_from_bview_default_string_impl(
        customization_point_type<T>,
        const bview& b,
        priority_tag<2>) noexcept
{
    if (!holds_string(b)) [[unlikely]]
        return nonstd::make_unexpected(conversion_errc::not_string_type);

    const auto& bstring = get_string(b);
    try { return T(rng::begin(bstring), rng::end(bstring)); }
    catch (...) { return nonstd::make_unexpected(conversion_errc::construction_error); }
}

// Conversion through std::string_view.

template <typename T>
    requires std::constructible_from<T, std::string_view>
constexpr nonstd::expected<T, conversion_errc>
convert_from_bview_default_string_impl(
        customization_point_type<T>,
        const bview& b,
        priority_tag<1>) noexcept

{
    if (!holds_string(b)) [[unlikely]]
        return nonstd::make_unexpected(conversion_errc::not_string_type);
    try { return T(static_cast<std::string_view>(get_string(b))); }
    catch (...) { return nonstd::make_unexpected(conversion_errc::construction_error); }
}


// Conversion of  byte stringsauto

template <typename T>
requires std::same_as<rng::range_value_t<T>, std::byte> &&
        std::constructible_from<T, const std::byte*, const std::byte*>
constexpr nonstd::expected<T, conversion_errc>
convert_from_bview_default_string_impl(
        customization_point_type<T>,
        const bview& b,
        priority_tag<2>) noexcept
{
    if (!holds_string(b)) [[unlikely]]
        return nonstd::make_unexpected(conversion_errc::not_string_type);

    const auto& bstring = get_string(b);
    try { return T(reinterpret_cast<const std::byte*>(rng::data(bstring)),
                   reinterpret_cast<const std::byte*>(rng::data(bstring)+rng::size(bstring))); }
    catch (...) { return nonstd::make_unexpected(conversion_errc::construction_error); }
}


// Conversion to list like types supporting front_inserter/back_inserter/inserter.

template <typename T>
requires std::constructible_from<T> &&
         rng::range<T> &&
         retrievable_from_bview<rng::range_value_t<T>> &&
        (supports_back_inserter<T> || supports_front_inserter<T> || supports_inserter<T>)
constexpr nonstd::expected<T, conversion_errc>
convert_from_bview_default_list_impl(
        customization_point_type<T>,
        const bview& bv,
        priority_tag<1>) noexcept
{
    if (!holds_list(bv)) [[unlikely]]
        return nonstd::make_unexpected(conversion_errc::not_list_type);

    auto& blist = get_list(bv);

    T value {};
    if constexpr (rng::sized_range<T> && has_reserve_member<T>) {
        value.reserve(rng::size(blist));
    }
    auto func = [](auto&& x) -> decltype(auto) {
        return get_as<rng::range_value_t<T>>(x);
    };
    try {
        if constexpr (supports_back_inserter<T>) {
            std::transform(rng::begin(blist), rng::end(blist), std::back_inserter(value), func);
        }
        else if constexpr (supports_front_inserter<T>) {
            std::transform(rng::rbegin(blist), rng::rend(blist), std::front_inserter(value), func);
        }
        else if constexpr (supports_inserter<T>) {
            std::transform(rng::begin(blist), rng::end(blist),
                           std::inserter(value, rng::begin(value)), func);
        }
    } catch (const bad_conversion& e) {
        return nonstd::make_unexpected(e.errc());
    } catch (...) {
        return nonstd::make_unexpected(conversion_errc::construction_error);
    }
    return value;
}

// Conversion to std::tuple, std::pair, array

template <typename T>
requires has_tuple_like_structured_binding<T> &&
        convertible_from_bview_to_tuple_elements<T>
constexpr nonstd::expected<T, conversion_errc>
convert_from_bview_default_list_impl(
        customization_point_type<T>,
        const bview& b,
        priority_tag<0>) noexcept
{
    if (!holds_list(b)) [[unlikely]]
        return nonstd::make_unexpected(conversion_errc::not_list_type);

    auto& blist = get_list(b);
    if (rng::size(blist) != std::tuple_size_v<T>) [[unlikely]] {
        return nonstd::make_unexpected(conversion_errc::size_mismatch);
    }
    T out {};

    try {
        std::apply(
                [&]<std::size_t... IS>(std::index_sequence<IS...>&&) constexpr {
                    using std::get;

                    ( (get<IS>(out) = get_as<std::tuple_element_t<IS, T>>(blist[IS])) , ... );
                },
                std::forward_as_tuple(std::make_index_sequence<std::tuple_size_v<T>>{})
        );
    } catch (const bad_conversion& e) {
        return nonstd::make_unexpected(conversion_errc::list_value_type_construction_error);
    } catch (...) {
        return nonstd::make_unexpected(conversion_errc::construction_error);
    }
    return out;
}

// Dict conversion

template <typename T>
requires key_value_associative_container<T> &&
        retrievable_from_bview<typename T::mapped_type> &&
        std::constructible_from<typename T::key_type, string_bview>
inline constexpr nonstd::expected<T, conversion_errc>
convert_from_bview_default_dict_impl(
        customization_point_type<T>,
        const bview& b,
        priority_tag<0>) noexcept
{
    if (!holds_dict(b)) [[unlikely]]
        return nonstd::make_unexpected(conversion_errc::not_dict_type);

    auto& bdict = get_dict(b);
    T out{};

    try {
        for ( auto&& [k, v] : bdict) {
            out.emplace(k, get_as<typename T::mapped_type>(v));
        }
    } catch (const bad_conversion& e) {
        return nonstd::make_unexpected(conversion_errc::dict_mapped_type_construction_error);
    } catch (...) {
        return nonstd::make_unexpected(conversion_errc::construction_error);
    }
    return out;
}

// multimap style

template <typename T>
requires key_value_associative_container<T> &&
        retrievable_from_bview<typename T::mapped_type> &&
        std::constructible_from<typename T::key_type, string_bview> &&
        ( is_instantiation_of_v<std::multimap, T> ||
          is_instantiation_of_v<std::unordered_multimap, T>)
inline constexpr nonstd::expected<T, conversion_errc>
convert_from_bview_default_dict_impl(
        customization_point_type<T>,
        const bview& b,
        priority_tag<0>) noexcept
{
    if (!holds_dict(b)) [[unlikely]]
        return nonstd::make_unexpected(conversion_errc::not_dict_type);

    T out{};
    auto& bdict = get_dict(b);

    try {
        for (auto&& [k, v] : bdict) {
            if (holds_list(v)) {
                for (auto&& v2 : get_list(v)) {
                    out.emplace(k, get_as<typename T::mapped_type>(v2));
                }
            }
            else {
                out.emplace(k, get_as<typename T::mapped_type>(v));
            }
        }
    }
    catch (const bad_conversion& e) {
        return nonstd::make_unexpected(conversion_errc::dict_mapped_type_construction_error);
    } catch (...) {
        return nonstd::make_unexpected(conversion_errc::construction_error);
    }
    return out;
}

template <typename T>
    requires requires () { typename std::pointer_traits<T>::element_type; }
inline nonstd::expected<T, conversion_errc>
convert_from_bview_to_pointer_impl(customization_point_type<T>, const bview& b) noexcept
{
    using E = typename std::pointer_traits<T>::element_type;

    // default construct pointer type
    auto result = convert_from_bview_to<E>(b);
    if (!result.has_value()) {
        return nonstd::make_unexpected(result.error());
    }

    using E = typename std::pointer_traits<T>::element_type;

    if constexpr (std::same_as<T, std::unique_ptr<E>>) {
        return std::make_unique<E>(std::move(*result));
    }
    else if constexpr (std::same_as<T, std::shared_ptr<E>>) {
        return std::make_shared<E>(std::move(*result));
    }
    else {
       return T(new E(std::move(*result)));
    }
}


template <typename T>
constexpr nonstd::expected<T, conversion_errc>
convert_from_bview_to_impl_dispatcher(customization_point_type<T>, const bview& b) noexcept
{
    // Use bencode::serialisation_traits to split the overload set per bencode_type bvalue.
    // This makes build errors easier to debug since we have to check less candidates.
    if constexpr (serialization_traits<T>::type == bencode_type::integer) {
        return convert_from_bview_default_integer_impl<T>(customization_for<T>, b);
    }
    else if constexpr (serialization_traits<T>::type == bencode_type::string) {
        return convert_from_bview_default_string_impl(customization_for<T>, b, priority_tag<5>{});
    }
    else if constexpr (serialization_traits<T>::type == bencode_type::list) {
        return convert_from_bview_default_list_impl(customization_for<T>, b, priority_tag<5>{});
    }
    else if constexpr (serialization_traits<T>::type == bencode_type::dict) {
        return convert_from_bview_default_dict_impl(customization_for<T>, b, priority_tag<5>{});
    }
    else {
        static_assert(detail::always_false<T>::value,
                "no serializer for T found, check if the correct trait class is included!");
    }
}

template <serializable T>
constexpr nonstd::expected<T, conversion_errc>
convert_from_bview_to(const bview& ref)
{
    // Check if there is a user-provided specialization found by ADL.
    if constexpr (conversion_from_bview_is_adl_overloaded<T>) {
        return bencode_convert_from_bview(customization_for<T>, ref);
    }
    else if constexpr (serialization_traits<T>::is_pointer) {
        return convert_from_bview_to_pointer_impl(customization_for<T>, ref);
    }
    else {
       return convert_from_bview_to_impl_dispatcher(customization_for<T>, ref);
    }

    return nonstd::make_unexpected(conversion_errc::undefined_conversion);
}

} // namespace bencode::detail
