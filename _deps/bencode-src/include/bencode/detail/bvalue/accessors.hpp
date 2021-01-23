#pragma once

#include <iosfwd>
#include <variant>
#include <type_traits>
#include <limits>
#include <compare>

#include <fmt/format.h>
#include <fmt/ostream.h>

#include "bencode/detail/utils.hpp"
#include "bencode/detail/bvalue/concepts.hpp"
#include "bencode/detail/bvalue/bvalue_policy.hpp"
#include "bencode/detail/bencode_type.hpp"
#include "bencode/detail/bad_conversion.hpp"
#include "bencode/detail/bvalue/bad_bvalue_access.hpp"
#include "bencode/detail/bvalue/basic_bvalue.hpp"
#include "bencode/detail/bvalue/conversion.hpp"

/// @file


namespace bencode {


/// Provides compile-time tag based access to the types of the of the alternatives
/// of the possibly cv-qualified basic_bvalue, combining cv-qualifications of the
/// basic_bvalue (if any) with the cv-qualifications of the alternative.
/// @tparam E bencode data type to check storage type for
/// @tparam T basic_bvalue template instantiation
template <bencode_type E, basic_bvalue_instantiation T>
struct bvalue_alternative
{
    using type = std::variant_alternative_t<
            static_cast<std::underlying_type_t<bencode_type>>(E),
            typename T::storage_type>;
};

template <bencode_type E, basic_bvalue_instantiation T>
struct bvalue_alternative<E, const T>
{ using type = std::add_const_t<typename bvalue_alternative<E, T>::type>; };

template <bencode_type E, basic_bvalue_instantiation T>
struct bvalue_alternative<E, volatile T>
{ using type = std::add_volatile_t<typename bvalue_alternative<E, T>::type>; };

template <bencode_type E, basic_bvalue_instantiation T>
struct bvalue_alternative<E, const volatile T>
{ using type = std::add_cv_t<typename bvalue_alternative<E, T>::type>; };

/// Helper template alias for bvalue_alternative
template <bencode_type E, basic_bvalue_instantiation T>
using bvalue_alternative_t = typename bvalue_alternative<E, T>::type;

namespace detail {

template <typename Policy>
constexpr const auto& get_storage(const basic_bvalue<Policy>& value) noexcept
{ return value.storage_; }

template <typename Policy>
constexpr auto& get_storage(basic_bvalue<Policy>& value) noexcept
{ return value.storage_; }

template <typename Policy>
constexpr const auto* get_storage(const basic_bvalue<Policy>* value) noexcept
{ return &(value->storage_); }

template <typename Policy>
constexpr auto* get_storage(basic_bvalue<Policy>* value) noexcept
{ return &(value->storage_); }

} // namespace detail


/// Enum based bvalue accessor.
/// If v holds an alternative of type category E,
/// returns a reference to the storage type used for the for type category E.
/// Otherwise, throws bad_bvalue_access.
/// @param v reference to a basic_value<Policy> instance
/// @throws bad_bvalue_access when current active alternative is not of category E.
template <enum bencode_type E, typename BV>
/// \cond CONCEPTS
    requires basic_bvalue_instantiation<BV>
/// \endcond
constexpr decltype(auto) get(BV&& v)
{
    using T = bvalue_alternative_t<E, std::remove_cvref_t<BV>>;
    auto* ptr = std::get_if<T>(&detail::get_storage(v));
    if (!ptr) [[unlikely]]
        throw bad_bvalue_access(fmt::format("bvalue is not of type: {}", to_string(E)));
    return detail::forward_like<BV>(*ptr);
}


/// Type based bvalue accessor.
/// If v holds a alternative of type T, returns a reference to the value stored in the
/// basic_bvalue instance. Otherwise, throws bad_bvalue_access.
/// @param v reference to a basic_value<Policy> instance
/// @returns a reference to the value stored in the basic_bvalue instance.
/// @throws bad_bvalue_access when the current active alternative type is not of type T.
template <typename BV, typename T>
/// \cond CONCEPTS
    requires basic_bvalue_instantiation<BV> &&
             bvalue_alternative_type<T, BV>
/// \endcond
constexpr decltype(auto) get(BV&& v)
{
    static_assert(!std::is_const_v<T>, "T cannot have cv-qualifiers");
    static_assert(!std::is_reference_v<T>, "T cannot be a reference");
    using traits = typename std::remove_cvref_t<BV>::template traits_type<T>;

    auto* ptr = std::get_if<T>(&detail::get_storage(v));
    if (!ptr) [[unlikely]]
        throw bad_bvalue_access(fmt::format("bvalue is not of type: {}",  to_string(traits::token)));
    return detail::forward_like<BV>(*ptr);
}



/// Enum based non-throwing bvalue accessor.
/// If pv is not nullptr and holds an value of type category E,
/// returns a pointer to the type stored in pv. Otherwise return a nullptr.
/// @param pv pointer to a basic_value<Policy> instance
/// @returns a pointer to the type stored in value if the active alternative
///         is of category E or a nullptr
template <enum bencode_type E, basic_bvalue_instantiation BV>
constexpr bvalue_alternative_t<E, BV>* get_if(BV* pv)
{
    using T = bvalue_alternative_t<E, BV>;
    return std::get_if<T>(detail::get_storage(pv));
}

/// Eum based non-throwing bvalue accessor.
/// If pv is not nullptr and holds a alternative of type T, returns a pointer to the type stored in value.
/// Otherwise return a nullptr.
/// @param pv const pointer to a basic_value<Policy> instance
/// @returns a const pointer to the type stored in value if E matches the active alternative or a nullptr
template <enum bencode_type E, basic_bvalue_instantiation BV>
constexpr const bvalue_alternative_t<E, BV>* get_if(const BV* pv)
{
    using T = bvalue_alternative_t<E, BV>;
    return std::get_if<T>(detail::get_storage(pv));
}

//
template <typename T, basic_bvalue_instantiation BV>
/// \cond CONCEPTS
    requires bvalue_alternative_type<T, BV>
/// \endcond
constexpr auto* get_if(BV* value) {
    return std::get_if<T>(detail::get_storage(value));
}

template <basic_bvalue_instantiation BV, bvalue_alternative_type<BV> T >
constexpr const auto* get_if(const BV* value) {
    return std::get_if<T>(detail::get_storage(value));
}



/// Non-throwing converting accessor.
/// Try to convert a basic_bvalue instantiation to T.
/// @returns expected object containing the value or an conversion_errc.
template <typename T, basic_bvalue_instantiation BV>
/// \cond CONCEPTS
requires retrievable_from_bvalue_for<T, typename std::remove_cvref_t<BV>::policy_type>
/// \endcond
constexpr auto try_get_as(BV&& value) -> nonstd::expected<T, conversion_errc>
{
    static_assert(!std::is_reference_v<T>, "T cannot have cv-qualifiers");

    if constexpr (bvalue_alternative_type<T, BV>) {
        // do not bind to const auto* since then move propagation wont work
        if (auto* ptr = get_if<serialization_traits<T>::type>(&value); ptr)
            return detail::forward_like<BV>(*ptr);
    }
    return detail::convert_from_bvalue_to<T>(std::forward<BV>(value));
}

/// Throwing converting accessor.
/// Try to convert a basic_bvalue instantiation to T.
/// @returns the converted value
/// @throws bad_conversion when the value could not be converted to the requested type.
template <typename T, basic_bvalue_instantiation BV>
/// \cond CONCEPTS
    requires retrievable_from_bvalue_for<T, typename std::remove_cvref_t<BV>::policy_type>
/// \endcond
constexpr auto get_as(BV&& value) -> T
{
    auto v = try_get_as<T>(std::forward<BV>(value));
    if (!v)
        throw bad_conversion(v.error());
    return std::move(*v);
}


/// Alias for bencode::get<bencode_type::integer>
template <typename Policy>
constexpr const auto& get_integer(const basic_bvalue<Policy>& value)
{ return bencode::get<bencode_type::integer>(value); }

/// Alias for bencode::get<bencode_type::integer>
template <typename Policy>
constexpr auto& get_integer(basic_bvalue<Policy>& value)
{ return bencode::get<bencode_type::integer>(value); }

/// Alias for bencode::get<bencode_type::integer>
template <typename Policy>
constexpr auto&& get_integer(basic_bvalue<Policy>&& value)
{ return bencode::get<bencode_type::integer>(std::move(value)); }

/// Alias for bencode::get_if<bencode_type::integer>
template <typename Policy>
constexpr const auto* get_if_integer(const basic_bvalue<Policy>* value)
{ return bencode::get_if<bencode_type::integer>(value); }

/// Alias for bencode::get_if<bencode_type::integer>
template <typename Policy>
constexpr auto* get_if_integer(basic_bvalue<Policy>* value)
{ return bencode::get_if<bencode_type::integer>(value); }

/// Alias for bencode::get<bencode_type::string>
template <typename Policy>
constexpr const auto& get_string(const basic_bvalue<Policy>& value)
{ return bencode::get<bencode_type::string>(value); }

/// Alias for bencode::get<bencode_type::string>
template <typename Policy>
constexpr auto& get_string(basic_bvalue<Policy>& value)
{ return bencode::get<bencode_type::string>(value); }

/// Alias for bencode::get_if<bencode_type::string>
template <typename Policy>
constexpr auto&& get_string(basic_bvalue<Policy>&& value)
{ return bencode::get<bencode_type::string>(std::move(value)); }

/// Alias for bencode::get_if<bencode_type::string>
template <typename Policy>
constexpr const auto* get_if_string(const basic_bvalue<Policy>* value)
{ return bencode::get_if<bencode_type::string>(value); }

/// Alias for bencode::get_if<bencode_type::string>
template <typename Policy>
constexpr auto* get_if_string(basic_bvalue<Policy>* value)
{ return bencode::get_if<bencode_type::string>(value); }

/// Alias for bencode::get<bencode_type::list>
template <typename Policy>
constexpr const auto& get_list(const basic_bvalue<Policy>& value)
{ return bencode::get<bencode_type::list>(value); }

/// Alias for bencode::get<bencode_type::list>
template <typename Policy>
constexpr auto& get_list(basic_bvalue<Policy>& value)
{ return bencode::get<bencode_type::list>(value); }

/// Alias for bencode::get<bencode_type::list>
template <typename Policy>
constexpr auto&& get_list(basic_bvalue<Policy>&& value)
{ return bencode::get<bencode_type::list>(std::move(value)); }

/// Alias for bencode::get_if<bencode_type::list>
template <typename Policy>
constexpr const auto* get_if_list(const basic_bvalue<Policy>* value)
{ return bencode::get_if<bencode_type::list>(value); }

/// Alias for bencode::get_if<bencode_type::list>
template <typename Policy>
constexpr auto* get_if_list(basic_bvalue<Policy>* value)
{ return bencode::get_if<bencode_type::list>(value); }

/// Alias for bencode::get<bencode_type::dict>
template <typename Policy>
constexpr const auto& get_dict(const basic_bvalue<Policy>& value)
{ return bencode::get<bencode_type::dict>(value); }

/// Alias for bencode::get<bencode_type::dict>(value)
template <typename Policy>
constexpr auto& get_dict(basic_bvalue<Policy>& value)
{ return bencode::get<bencode_type::dict>(value); }

/// Alias for bencode::get<bencode_type::dict>
template <typename Policy>
constexpr auto&& get_dict(basic_bvalue<Policy>&& value)
{ return bencode::get<bencode_type::dict>(std::move(value)); }

/// Alias for bencode::get_if<bencode_type::dict>
template <typename Policy>
constexpr const auto* get_if_dict(const basic_bvalue<Policy>* value)
{ return bencode::get_if<bencode_type::dict>(value); }

/// Alias for bencode::get_if<bencode_type::dict>
template <typename Policy>
constexpr auto* get_if_dict(basic_bvalue<Policy>* value)
{ return bencode::get_if<bencode_type::dict>(value); }


/// Returns true if the type of the current alternative is equal to E.
/// @param bv a basic_bvalue instance.
template <enum bencode_type E, typename Policy>
constexpr bool holds_alternative(const basic_bvalue<Policy>& bv)
{
    using T = bvalue_alternative_t<E, basic_bvalue<Policy>>;
    return std::holds_alternative<T>(detail::get_storage(bv));
}

template <typename Policy>
constexpr bool holds_uninitialized(const basic_bvalue<Policy>& value) noexcept
{ return holds_alternative<bencode_type::uninitialized>(value); }

template <typename Policy>
constexpr bool holds_integer(const basic_bvalue<Policy>& value) noexcept
{ return holds_alternative<bencode_type::integer>(value); }

template <typename Policy>
constexpr bool holds_string(const basic_bvalue<Policy>& value) noexcept
{ return holds_alternative<bencode_type::string>(value); }

template <typename Policy>
constexpr bool holds_list(const basic_bvalue<Policy>& value) noexcept
{ return holds_alternative<bencode_type::list>(value); }

template <typename Policy>
constexpr bool holds_dict(const basic_bvalue<Policy>& value) noexcept
{ return holds_alternative<bencode_type::dict>(value); }


} // namespace bencode