#pragma once

#include <type_traits>
#include <fmt/format.h>
#include <fmt/ostream.h>

#include "bencode/detail/bencode_type.hpp"
#include "bencode/detail/bad_conversion.hpp"
#include "bencode/detail/symbol.hpp"
#include "bencode/detail/utils.hpp"

#include "bencode/detail/bview/bad_bview_access.hpp"
#include "bencode/detail/bview/concepts.hpp"
#include "bencode/detail/bview/bview.hpp"

/// @file
/// Provide accessors for bview, integer_bview, string_bview, list_bview and dict_bview types.

namespace bencode {

namespace detail {

constexpr const descriptor* get_storage(const bview& value) noexcept
{ return value.desc_; }

// forward declaration
template <typename T>
constexpr nonstd::expected<T, conversion_errc> convert_from_bview_to(const bview& desc);
}

/// Provides compile-time tag based access to the alternative interfaces to bview.
/// @tparam E enum representing the bencode data type.
/// @returns the matching bview class for a given bencode data type
template <bencode_type E>
struct bview_alternative
{
private:
    using alterative_typelist= std::tuple<
            integer_bview,
            string_bview,
            list_bview,
            dict_bview>;
public:
    using type = std::tuple_element_t<static_cast<std::size_t>(E)-1, alterative_typelist>;
};

/// Helper alias template for bview_alternative
template <bencode_type token>
using bview_alternative_t = typename bview_alternative<token>::type;



/// Enum based bview accessor.
/// If v holds the alternative type specified b, returns a reference to the bview class for the type stored in v.
/// Otherwise, throws bad_bview_access.
/// @tparam E type to look up
/// @param v reference to a bview
/// @returns reference to the value described in the view.
template <enum bencode_type E>
constexpr bview_alternative_t<E>& get(bview& v)
{
    using D = typename bview_alternative<E>::type;
    if (v.type() != E) [[unlikely]]
        throw bad_bview_access(fmt::format("bvalue is not of type: {}\"", E));
    return static_cast<D&>(v);
}

/// Enum based bview accessor.
/// If v holds the alternative type specified b, returns a reference to the bview class for the type stored in v.
/// Otherwise, throws bad_bview_access.
/// @tparam E type to look up
/// @param v const reference to a bview
/// @returns const reference to the value described in the view.
template <enum bencode_type E>
constexpr const bview_alternative_t<E>& get(const bview& v)
{
    using D = typename bview_alternative<E>::type;
    if (v.type() != E) [[unlikely]]
        throw bad_bview_access(fmt::format("bvalue is not of type: {}\"", E));
    return static_cast<const D&>(v);
}

/// Type based bview accessor.
/// If v holds the alternative type specified b, returns a reference to the bview class for the type stored in v.
/// Otherwise, throws bad_bview_access.
/// @tparam E type to look up
/// @param v reference to a bview
/// @returns reference to the value described in the view.
template <typename T>
/// \cond CONCEPTS
    requires bview_alternative_type<T>
/// \endcond
constexpr T& get(bview& v)
{ return get<serialization_traits<T>::type>(v); }

/// Enum based bview accessor.
/// If v holds the alternative type specified b, returns a reference to the bview class for the type stored in v.
/// Otherwise, throws bad_bview_access.
/// @tparam E type to look up
/// @param v const reference to a bview
/// @returns const reference to the value described in the view.
template <typename T>
/// \cond CONCEPTS
requires bview_alternative_type<T>
/// \endcond
constexpr const T& get(const bview& v)
{ return get<serialization_traits<T>::type>(v); }

/// Enum-based non-throwing accessor.
/// If pv is not a null pointer and pv->type() == E, returns a pointer to the bview described in the bview.
/// Otherwise, returns a null pointer.
/// @param pv pointer to a bview
/// @returns A pointer to the value decribed by the bview, nullptr on error.
template <enum bencode_type E>
constexpr bview_alternative_t<E>* get_if(bview* pv) noexcept
{
    using D = typename bview_alternative<E>::type;
    if (pv == nullptr || pv->type() != E) [[unlikely]] return nullptr;
    return static_cast<D*>(pv);
}

/// Enum-based non-throwing accessor.
/// If pv is not a null pointer and pv->type() == E, returns a pointer to the bview described in the bview.
/// Otherwise, returns a null pointer.
/// @param pv const pointer to a bview
/// @returns A const pointer to the value decribed by the bview, nullptr on error.
template <enum bencode_type E>
constexpr const bview_alternative_t<E>* get_if(const bview* desc) noexcept
{
    using D = typename bview_alternative<E>::type;
    if (desc->type() != E) [[unlikely]] return nullptr;
    return static_cast<const D*>(desc);
}


/// Type-based non-throwing accessor.
/// If pv is not a null pointer and pv is of type T, returns a pointer to T.
/// Otherwise, returns a null pointer.
/// @param pv pointer to a bview
/// @returns a pointer to T
template <typename T>
/// \cond CONCEPTS
requires bview_alternative_type<T>
/// \endcond
constexpr T* get_if(bview* pv) noexcept
{
    if (pv == nullptr || pv->type() != serialization_traits<T>::type) [[unlikely]] return nullptr;
    return static_cast<T*>(pv);
}

/// Type-based non-throwing accessor.
/// If pv is not a null pointer and pv is of type T, returns a const pointer to T.
/// Otherwise, returns a null pointer.
/// @param pv pointer to a bview
/// @returns a const pointer to T
template <typename T>
/// \cond CONCEPTS
requires bview_alternative_type<T>
/// \endcond
constexpr const T* get_if(const bview* pv) noexcept
{
    if (pv == nullptr || pv->type() != serialization_traits<T>::type) [[unlikely]] return nullptr;
    return static_cast<const T*>(pv);
}


/// Alias for get<bencode_type::integer>(desc)
constexpr const integer_bview& get_integer(const bview& desc)
{ return get<bencode_type::integer>(desc); }

/// Alias for get<bencode_type::integer>(desc)
constexpr integer_bview& get_integer(bview& desc)
{ return get<bencode_type::integer>(desc); }

/// Alias for get_if<bencode_type::integer>(desc)
constexpr const integer_bview* get_if_integer(const bview* desc) noexcept
{ return get_if<bencode_type::integer>(desc); }

/// Alias for get_if<bencode_type::integer>(desc)
constexpr integer_bview* get_if_integer(bview* desc) noexcept
{ return get_if<bencode_type::integer>(desc); }

/// Alias for get<bencode_type::string>(desc)
constexpr const string_bview& get_string(const bview& desc)
{ return get<bencode_type::string>(desc); }

/// Alias for get<bencode_type::string>(desc)
constexpr string_bview& get_string(bview& desc)
{ return get<bencode_type::string>(desc); }

/// Alias for get_if<bencode_type::string>(desc)
constexpr const string_bview* get_if_string(const bview* desc) noexcept
{ return get_if<bencode_type::string>(desc); }

/// Alias for get_if<bencode_type::string>(desc)
constexpr string_bview* get_if_string(bview* desc) noexcept
{ return get_if<bencode_type::string>(desc); }

/// Alias for get<bencode_type::list>(desc)
constexpr const list_bview& get_list(const bview& desc)
{ return get<bencode_type::list>(desc); }

/// Alias for get<bencode_type::list>(desc)
constexpr list_bview& get_list(bview& desc)
{ return get<bencode_type::list>(desc); }

/// Alias for get_if_list<bencode_type::list>(desc)
constexpr const list_bview* get_if_list(const bview* desc) noexcept
{ return get_if<bencode_type::list>(desc); }

/// Alias for get_if_list<bencode_type::list>(desc)
constexpr list_bview* get_if_list(bview* desc) noexcept
{ return get_if<bencode_type::list>(desc); }

/// Alias for get<bencode_type::dict>(desc)
constexpr const dict_bview& get_dict(const bview& desc)
{ return get<bencode_type::dict>(desc); }

/// Alias for get<bencode_type::dict>(desc)
constexpr dict_bview& get_dict(bview& desc)
{ return get<bencode_type::dict>(desc); }

/// Alias for get_if<bencode_type::dict>(desc)
constexpr const dict_bview* get_if_dict(const bview* desc) noexcept
{ return get_if<bencode_type::dict>(desc); }

/// Alias for get_if<bencode_type::dict>(desc)
constexpr dict_bview* get_if_dict(bview* desc) noexcept
{ return get_if<bencode_type::dict>(desc); }



/// Check if the bview described a bencode data type category E.
/// \tparam E the bencode data type to check for
/// \param v the bview to check
/// \return true if the bview described the given bencode data type E, false otherwise.
template <enum bencode_type E>
constexpr bool holds_alternative(const bview& v) noexcept
{
    auto s = detail::get_storage(v);
    if constexpr (E == bencode_type::uninitialized) {
        if (s == nullptr) return true;
    }
    return s && s->type() == E;
}

/// Check if the bview describes a bencode data type of type T
/// @tparam E the bencode data type to check for
/// @param v the bview to check
/// @return true if the bview described the given bencode data type E, false otherwise.
template <typename T>
/// \cond CONCEPTS
requires bview_alternative_type<T>
/// \endcond
constexpr bool holds_alternative(const bview& v) noexcept
{
    auto s = detail::get_storage(v);
    return s && s->type() == serialization_traits<T>::type;
}

/// Alias for holds_alternative<bencode_type::unknown>(v).
/// @note this is only true for a default constructed bview.
constexpr bool holds_uninitialized(const bview& v) noexcept
{ return detail::get_storage(v) == nullptr; }

/// Alias for holds_alternative<bencode_type::integer>(v).
constexpr bool holds_integer(const bview& v) noexcept
{
    return holds_alternative<bencode_type::integer>(v);
}

/// Alias for holds_alternative<bencode_type::string>(v).
constexpr bool holds_string(const bview& v) noexcept
{
    return holds_alternative<bencode_type::string>(v);
}

/// Alias for holds_alternative<bencode_type::list>(v).
constexpr bool holds_list(const bview& v) noexcept
{
    return holds_alternative<bencode_type::list>(v);
}

/// Alias for holds_alternative<bencode_type::dict>(v).
constexpr bool holds_dict(const bview& v) noexcept
{
    return holds_alternative<bencode_type::dict>(v);
}


/// Throwing converting bview accessor.
/// If serialization_traits<T>::type == bview.type(), converts bvalue to T.
/// If the active alternative type could not be converted returns bad_conversion.
/// @tparam the type to convert to
/// @param v reference to bview
/// @returns an expected instance with the returned value or an error code.
template <typename T>
/// \cond CONCEPTS
requires retrievable_from_bview<T>
/// \endcond
constexpr nonstd::expected<T, conversion_errc> try_get_as(const bview& v)
{
    static_assert(!std::is_reference_v<T>, "T cannot have cv-qualifiers");

    if constexpr (std::is_same_v<T, std::int64_t>)
        return static_cast<T>(get_integer(v));
    if constexpr (std::is_same_v<T, std::string_view>)
        return static_cast<T>(get_string(v));

    return detail::convert_from_bview_to<T>(v);
}


/// Throwing converting accessor.
/// Try to convert a basic_bvalue instantiation to T.
/// @returns the converted value
/// @throws bad_conversion when the value could not be converted to the requested type.
template <typename T>
/// \cond CONCEPTS
requires retrievable_from_bview<T>
/// \endcond
constexpr T get_as(const bview& value)
{
    auto v = try_get_as<T>(value);
    if (!v) throw bad_conversion(v.error());
    return std::move(*v);
}

} // namespace bencode