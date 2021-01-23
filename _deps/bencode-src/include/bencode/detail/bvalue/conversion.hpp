
#pragma once

#include <nonstd/expected.hpp>
#include <span>

#include "bencode/detail/symbol.hpp"
#include "bencode/detail/utils.hpp"
#include "bencode/detail/concepts.hpp"
#include "bencode/detail/bad_conversion.hpp"

#include "bencode/detail/bvalue/concepts.hpp"
#include "bencode/detail/bvalue/bvalue_policy.hpp"

namespace bencode::detail {

namespace rng = std::ranges;

template <typename T, basic_bvalue_instantiation U>
constexpr nonstd::expected<T, conversion_errc>
convert_from_bvalue_integer_impl(customization_point_type<T>, U&& v, priority_tag<0>) noexcept
{
    if (!holds_integer(v)) [[unlikely]]
        return nonstd::make_unexpected(conversion_errc::not_integer_type);
    return static_cast<T>(detail::forward_like<U>(get_integer(v)));
}


// c_str member conversion

template <typename Policy>
    requires has_c_str_member<policy_string_t<Policy> >
inline nonstd::expected<const char*, conversion_errc>
convert_from_bvalue_string_impl(customization_point_type<const char*>,
                                const basic_bvalue<Policy>& b,
                                priority_tag<0>) noexcept
{
    if (!holds_string(b)) [[unlikely]]
        return nonstd::make_unexpected(conversion_errc::not_string_type);
    const auto& s = get_string(b);
    return s.c_str();
}

// Conversion to T via pair of iterators

template <typename T, basic_bvalue_instantiation U, typename Policy = typename std::remove_cvref_t<U>::policy_type>
    requires
        std::constructible_from<typename policy_string_t<Policy>::value_type,
                                 rng::range_value_t<T>> &&
        std::constructible_from<T, rng::iterator_t<policy_string_t<Policy>>,
                                   rng::sentinel_t<policy_string_t<Policy>>>
constexpr nonstd::expected<T, conversion_errc>
convert_from_bvalue_string_impl(customization_point_type<T>,
                                U&& b,
                                priority_tag<2>) noexcept
{
    if (!holds_string(b)) [[unlikely]]
        return nonstd::make_unexpected(conversion_errc::not_string_type);

    auto& bstring = get_string(b);
    try { return T(rng::begin(bstring), rng::end(bstring)); }
    catch (...) { return nonstd::make_unexpected(conversion_errc::construction_error); }
}

// Conversion to T via pointer and size

template <typename T, basic_bvalue_instantiation U, typename Policy = typename std::remove_cvref_t<U>::policy_type>
    requires rng::contiguous_range<policy_string_t<Policy>> &&
             std::constructible_from<T, rng::range_value_t<policy_string_t<Policy>>*,
                                         rng::range_size_t<policy_string_t<Policy>>>
constexpr nonstd::expected<T, conversion_errc>
convert_from_bvalue_string_impl(customization_point_type<T>,
                                U&& b,
                                priority_tag<3>) noexcept
{
    if (!holds_string(b)) [[unlikely]]
        return nonstd::make_unexpected(conversion_errc::not_string_type);

    auto& bstring = get_string(b);
    try { return T(rng::data(bstring), rng::size(bstring)); }
    catch (...) { return nonstd::make_unexpected(conversion_errc::construction_error); }
}

// Conversion through std::string_view.

template <typename T, basic_bvalue_instantiation U, typename Policy = typename std::remove_cvref_t<U>::policy_type>
    requires std::constructible_from<T, std::string_view> &&
             std::convertible_to<policy_string_t<Policy>, std::string_view>
constexpr nonstd::expected<T, conversion_errc>
convert_from_bvalue_string_impl(customization_point_type<T>,
                                U&& b,
                                priority_tag<0>) noexcept

{
    if (!holds_string(b)) [[unlikely]]
        return nonstd::make_unexpected(conversion_errc::not_string_type);
    try { return T(static_cast<std::string_view>(get_string(b))); }
    catch (...) { return nonstd::make_unexpected(conversion_errc::construction_error); }
}


// Conversion of  byte strings auto

template <typename T, basic_bvalue_instantiation U, typename Policy = typename std::remove_cvref_t<U>::policy_type>
    requires std::same_as<std::remove_cv_t<rng::range_value_t<T>>, std::byte> &&
             std::constructible_from<T, rng::range_value_t<T>*, rng::range_value_t<T>*> &&
             rng::contiguous_range<policy_string_t<Policy>>
constexpr nonstd::expected<T, conversion_errc> convert_from_bvalue_string_impl(
        customization_point_type<T>,
        U&& b,
        priority_tag<0>) noexcept
{
    if (!holds_string(b)) [[unlikely]]
        return nonstd::make_unexpected(conversion_errc::not_string_type);

    auto& bstring = get_string(b);
    try { return T(reinterpret_cast<rng::range_value_t<T>*>(rng::data(bstring)),
                   reinterpret_cast<rng::range_value_t<T>*>(rng::data(bstring)+rng::size(bstring))); }
    catch (...) { return nonstd::make_unexpected(conversion_errc::construction_error); }
}

// Conversion to list like types supporting front_inserter/back_inserter/inserter.

template <typename T, typename U, typename Policy = typename std::remove_cvref_t<U>::policy_type>
    requires basic_bvalue_instantiation<U> &&
             std::constructible_from<T> &&
             rng::range<T> &&
             retrievable_from_bvalue_for<rng::range_value_t<T>, Policy> &&
             (supports_back_inserter<T> || supports_front_inserter<T> || supports_inserter<T>)
constexpr nonstd::expected<T, conversion_errc> convert_from_bvalue_list_impl(
        customization_point_type<T>,
        U&& bv,
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
        return get_as<rng::range_value_t<T>>(detail::forward_like<U>(x));
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

template <typename T, basic_bvalue_instantiation BV, typename U = std::remove_cvref_t<BV>>
    requires has_tuple_like_structured_binding<T> &&
             convertible_from_bvalue_to_tuple_elements<T, typename U::policy_type>
constexpr nonstd::expected<T, conversion_errc> convert_from_bvalue_list_impl(
            customization_point_type<T>,
            BV&& b,
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
                ( (get<IS>(out) = get_as<std::tuple_element_t<IS, T>>(
                                            detail::forward_like<BV>(blist[IS]))) , ... );
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

template <typename T, basic_bvalue_instantiation BV, typename Policy = typename std::remove_cvref_t<BV>::policy_type>
    requires key_value_associative_container<T> &&
             retrievable_from_bvalue_for<typename T::mapped_type, Policy> &&
             std::constructible_from<typename T::key_type, policy_string_t<Policy>>
inline constexpr nonstd::expected<T, conversion_errc>
convert_from_bvalue_dict_impl(
            customization_point_type<T>,
            BV&& b,
            priority_tag<0>) noexcept
{
    if (!holds_dict(b)) [[unlikely]]
        return nonstd::make_unexpected(conversion_errc::not_dict_type);

    auto& bdict = get_dict(b);
    T out{};

    try {
        for ( auto&& [k, v] : bdict) {
            out.emplace(
                    detail::forward_like<BV>(k),
                    get_as<typename T::mapped_type>(detail::forward_like<BV>(v)));
        }
    } catch (const bad_conversion& e) {
        return nonstd::make_unexpected(conversion_errc::dict_mapped_type_construction_error);
    } catch (...) {
        return nonstd::make_unexpected(conversion_errc::construction_error);
    }
    return out;
}

// multimap style

template <typename T, basic_bvalue_instantiation BV, typename Policy = typename std::remove_cvref_t<BV>::policy_type>
requires key_value_associative_container<T> &&
        retrievable_from_bvalue_for<typename T::mapped_type, Policy> &&
        std::constructible_from<typename T::key_type, policy_string_t<Policy>> &&
        ( is_instantiation_of_v<std::multimap, T> ||
          is_instantiation_of_v<std::unordered_multimap, T>)
inline constexpr nonstd::expected<T, conversion_errc>
convert_from_bvalue_dict_impl(
        customization_point_type<T>,
        BV&& b,
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
                    out.emplace(
                        detail::forward_like<BV>(k),
                        get_as<typename T::mapped_type>(detail::forward_like<BV>(v2)));
                }
            }
            else {
                out.emplace(
                     detail::forward_like<BV>(k),
                     get_as<typename T::mapped_type>(detail::forward_like<BV>(v)));
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

template <typename T, basic_bvalue_instantiation U>
inline nonstd::expected<T, conversion_errc>
convert_from_bvalue_to_pointer_impl(customization_point_type<T>, U&& bvalue) noexcept
{
    using E = typename std::pointer_traits<T>::element_type;

    // default construct pointer type
    auto result = convert_from_bvalue_to<E>(std::forward<U>(bvalue));
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


template <typename T, basic_bvalue_instantiation U>
constexpr nonstd::expected<T, conversion_errc>
convert_from_bvalue_to_impl_dispatcher(U&& bvalue) noexcept
{
    if constexpr (bencode::serialization_traits<T>::type == bencode_type::integer) {
        return convert_from_bvalue_integer_impl(
                customization_for<T>, std::forward<U>(bvalue), priority_tag<0>{});
    }
    else if constexpr (bencode::serialization_traits<T>::type == bencode_type::string) {
        return convert_from_bvalue_string_impl(
                customization_for<T>, std::forward<U>(bvalue), priority_tag<4>{});
    }
    else if constexpr (bencode::serialization_traits<T>::type == bencode_type::list) {
        return convert_from_bvalue_list_impl(
                customization_for<T>, std::forward<U>(bvalue), priority_tag<1>{});
    }
    else if constexpr (bencode::serialization_traits<T>::type == bencode_type::dict) {
        return convert_from_bvalue_dict_impl(
                customization_for<T>, std::forward<U>(bvalue), priority_tag<0>{});
    }
    else {
        static_assert(detail::always_false<T>::value,
                "no serializer for T found, check if the correct trait class is included!");
    }
}




template <serializable T, basic_bvalue_instantiation U>
constexpr nonstd::expected<T, conversion_errc>
convert_from_bvalue_to(U&& bvalue) noexcept
{
    using Policy = typename std::remove_cvref_t<U>::policy_type;

    // Check if there is a user-provided specialization found by ADL.
    if constexpr (conversion_from_bvalue_is_adl_overloaded<T, Policy>) {
        return bencode_convert_from_bvalue(customization_for<T>, std::forward<U>(bvalue));
    }
    else if constexpr (serialization_traits<T>::is_pointer) {
        return convert_from_bvalue_to_pointer_impl(customization_for<T>, std::forward<U>(bvalue));
    }
    // Use bencode::serialisation_traits to split the overload set per bencode_type bvalue.
    // This makes build errors easier to debug since we have to check less candidates.
    else {
        return convert_from_bvalue_to_impl_dispatcher<T>(std::forward<U>(bvalue));
    }
}



} // namespace bencode::detail