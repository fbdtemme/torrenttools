#pragma once

#include <compare>
#include <iosfwd>
#include <limits>
#include <type_traits>
#include <variant>


#include <fmt/format.h>
#include <fmt/ostream.h>

#include "bencode/detail/concepts.hpp"
#include "bencode/detail/utils.hpp"
#include "bencode/detail/bad_conversion.hpp"
#include "bencode/detail/bencode_type.hpp"
#include "bencode/detail/bvalue/bvalue_policy.hpp"
#include "bencode/detail/out_of_range.hpp"
#include "bencode/detail/bvalue/accessors.hpp"
#include "bencode/detail/bvalue/assignment.hpp"
#include "bencode/detail/bvalue/comparison.hpp"

namespace bencode {

namespace detail {
/// Helper function for private access to storage object
template <typename Policy>
constexpr const auto& get_storage(const basic_bvalue<Policy>& value) noexcept;

template <typename Policy>
constexpr auto& get_storage(basic_bvalue<Policy>& value) noexcept;

template <typename Policy>
constexpr const auto* get_storage(const basic_bvalue<Policy>* value) noexcept;

template <typename Policy>
constexpr auto* get_storage(basic_bvalue<Policy>* value) noexcept;

// forward declarations
template <typename U, typename Policy, typename T = std::remove_cvref_t<U>>
    requires serializable<T>
inline void assign_to_bvalue(basic_bvalue<Policy>& bvalue, U&& value);

template <basic_bvalue_instantiation BV>
decltype(auto) evaluate(const bpointer& pointer, BV&& bv);

template <basic_bvalue_instantiation BV>
bool contains(const bpointer& pointer, const BV& bv);

} // namespace detail

/// A class template storing a bencoded bvalue.
///
/// An instance of basic_bvalue at any given time either holds a bvalue of one of the
/// bencode data types, or uninitialized_type in the case of error or no bvalue.
/// A single bvalue can store large and complicated bencoded data structures consisting
/// of arbitrarily nested lists and dicts with many sub-values, that are again bvalue instances.
///
/// @tparam Policy instantation of bvalue_policy to defining the storage types
template <typename Policy>
class basic_bvalue
{
public:
    using policy_type        = Policy;

    // storage type aliasesPolicy
    using uninitialized_type = detail::policy_uninitialized_t<policy_type>;
    using integer_type       = detail::policy_integer_t<policy_type>;
    using string_type        = detail::policy_string_t<policy_type>;
    using list_type          = detail::policy_list_t<policy_type>;
    using dict_type          = detail::policy_dict_t<policy_type>;
    using string_init_list   = detail::policy_string_init_list_t<policy_type>;
    using list_init_list     = detail::policy_list_init_list_t<policy_type>;
    using dict_init_list     = detail::policy_dict_init_list_t<policy_type>;

    using reference          = basic_bvalue<policy_type>&;
    using pointer            = basic_bvalue<policy_type>*;
    using const_reference    = const basic_bvalue<policy_type>&;
    using const_pointer      = const basic_bvalue<policy_type>*;

    // iterator aliases
    using list_iterator               = typename list_type::iterator;
    using list_const_iterator         = typename list_type::const_iterator;
    using list_reverse_iterator       = typename list_type::reverse_iterator;
    using list_const_reverse_iterator = typename list_type::const_reverse_iterator;
    using dict_iterator               = typename dict_type::iterator;
    using dict_const_iterator         = typename dict_type::const_iterator;
    using dict_reverse_iterator       = typename dict_type::reverse_iterator;
    using dict_const_reverse_iterator = typename dict_type::const_reverse_iterator;

    using storage_type      = std::variant<uninitialized_type,
                                           integer_type,
                                           string_type,
                                           list_type,
                                           dict_type>;

    using dict_value_type   = typename dict_type::value_type;
    using dict_key_type     = typename dict_type::key_type;
    using list_value_type   = typename list_type::value_type;
    using string_value_type = typename string_type::value_type;

    constexpr basic_bvalue() noexcept = default;
    constexpr basic_bvalue(const basic_bvalue& other) noexcept(std::is_nothrow_copy_constructible_v<storage_type>) = default;
    constexpr basic_bvalue(basic_bvalue&& other) noexcept(std::is_nothrow_move_constructible_v<storage_type>) = default;

    basic_bvalue(dict_init_list il)
        : basic_bvalue(dict_type{})
    {
        auto& bdict = *std::get_if<dict_type>(&storage_);
        for (auto&& elem: il) {
            bdict[std::move(elem.first)] = std::move(elem.second);
        }
    }

    //===========================================================================//
    //    converting copy/move constructors                                      //
    //===========================================================================//

    /// Copy/move constructor for exact matches to one of the storage_types
    template <typename U, typename T = std::remove_cvref_t<U>>
    /// \cond CONCEPTS
        requires bvalue_alternative_type<T, basic_bvalue>
    /// \endcond
    constexpr basic_bvalue(U&& v)
    noexcept(noexcept(storage_type(std::in_place_type<std::remove_cvref_t<T>>, std::forward<U>(v))))
            : storage_(std::in_place_type<T>, std::forward<U>(v))
    {}

    /// Copy/move constructor for types that can be converted to bvalue using built-in conversions
    /// or used defined types that implement are convertible to bvalue though
    /// bencode_assign_to_bvalue().
    template <typename U, typename T = std::remove_cvref_t<U>>
    /// \cond CONCEPTS
        requires (!basic_bvalue_instantiation<T>) &&
                     (!bvalue_alternative_type<T, basic_bvalue>) &&
                     serializable<T>
    /// \endcond
    constexpr basic_bvalue(U&& v)
    noexcept(noexcept(detail::assign_to_bvalue(std::declval<basic_bvalue&>(), std::forward<U>(v))))
    {
        detail::assign_to_bvalue(*this, std::forward<U>(v));
    }

    template <bencode_type T, typename... Args>
    /// \cond CONCEPTS
    requires std::constructible_from<bvalue_alternative_t<T, basic_bvalue>, Args...>
    /// \endcond
    explicit constexpr basic_bvalue(bencode_type_tag<T>, Args&& ... args)
            : storage_(std::in_place_type<bvalue_alternative_t<T, basic_bvalue>>,
                       std::forward<Args>(args)...)
    {};

    template <bencode_type T, typename... Args>
    /// \cond CONCEPTS
        requires std::constructible_from<string_type, string_init_list, Args...> && (T == bencode_type::string)
    /// \endcond
    explicit constexpr basic_bvalue(bencode_type_tag<T>,
                               string_init_list il,
                               Args&& ... args)
        : storage_(std::in_place_type<bvalue_alternative_t<T, basic_bvalue>>,
                   std::move(il), std::forward<Args>(args)...)
    {};

    template <bencode_type T, typename... Args>
    /// \cond CONCEPTS
        requires std::constructible_from<list_type, list_init_list, Args...> && (T == bencode_type::list)
    /// \endcond
    explicit constexpr basic_bvalue(bencode_type_tag<T>,
                                    list_init_list il,
                                    Args&& ... args)
            : storage_(std::in_place_type<list_type>,
                       std::move(il), std::forward<Args>(args)...)
    {};

    template <bencode_type T, typename... Args, typename = std::enable_if_t<T == bencode_type::dict>>
    /// \cond CONCEPTS
        requires std::constructible_from<dict_type, dict_init_list, Args...>
    /// \endcond
    explicit constexpr basic_bvalue(bencode_type_tag<T>,
                                   dict_init_list il,
                                   Args&& ... args)
            : storage_(std::in_place_type<dict_type>,
                       std::move(il), std::forward<Args>(args)...)
    {};


//===========================================================================//
//    copy/move assignment and swap operators                                //
//===========================================================================//
public:
    // TODO: add converting assignment operator instead of current implicit construction + move

    constexpr basic_bvalue& operator=(const basic_bvalue& rhs) = default;
    constexpr basic_bvalue& operator=(basic_bvalue&& rhs) noexcept(std::is_nothrow_move_assignable_v<storage_type>) = default;

    template <typename T>
    /// \cond CONCEPTS
        requires bvalue_alternative_type<std::remove_cvref_t<T>, basic_bvalue>
    /// \endcond
    basic_bvalue& operator=(T&& rhs) {
        // only forward exact alternative types to the variant assignment operator
        // all other types will pass through the traits_old interface to determine
        // the result of an assignment operation.
        storage_.operator=(std::forward<T>(rhs));
        return *this;
    }


public:

    /// Discards the current active alternative and sets the storage to the uninitialized state.
    void discard()
    {
        storage_.template emplace<uninitialized_type>();
    }

    //===========================================================================//
    //  Emplace a variant type in an existing object                             //
    //===========================================================================//

    /// Construct an integer in place.
    /// @param args arguments to forward to the integer alternative constructor.
    /// @returns A reference to the new contained bvalue.
    template <typename... Args>
    /// \cond CONCEPTS
        requires std::constructible_from<integer_type, Args...>
    /// \endcond
    constexpr integer_type& emplace_integer(Args&&... args)
    {
        return storage_.template emplace<integer_type>(std::forward<Args>(args)...);
    }

    /// Construct a string in place.
    /// @param args arguments to forward to the integer alternative constructor.
    /// @returns A reference to the new contained bvalue.
    template <typename... Args>
    /// \cond CONCEPTS
        requires std::constructible_from<string_type, Args...>
    /// \endcond
    string_type& emplace_string(Args&&... args)
    {
        return storage_.template emplace<string_type>(std::forward<Args>(args)...);
    }

    /// @copydoc emplace_string()
    template <typename... Args>
    /// \cond CONCEPTS
        requires std::constructible_from<string_type, string_init_list, Args...>
    /// \endcond
    string_type& emplace_string(string_init_list il, Args&&... args)
    {
        return storage_.template emplace<string_type>(std::move(il), std::forward<Args>(args)...);
    }

    /// Construct a list in place.
    /// @param args arguments to forward to the list alternative constructor.
    /// @returns A reference to the new contained bvalue.
    template <typename... Args>
    /// \cond CONCEPTS
        requires std::constructible_from<list_type, Args...>
    /// \endcond
    list_type& emplace_list(Args&&... args)
    {
        return storage_.template emplace<list_type>(std::forward<Args>(args)...);
    }

    /// @copydoc emplace_list()
    template <typename... Args>
    /// \cond CONCEPTS
        requires std::constructible_from<list_type, list_init_list, Args...>
    /// \endcond
    list_type& emplace_list(list_init_list il, Args&&... args)
    {
        return storage_.template emplace<list_type>(std::move(il), std::forward<Args>(args)...);
    }

    /// Construct a dict in place.
    /// @param args arguments to forward to the dict alternative constructor.
    /// @returns A reference to the new contained bvalue.
    template <typename... Args>
    /// \cond CONCEPTS
        requires std::constructible_from<dict_type, Args...>
    /// \endcond
    dict_type& emplace_dict(Args&&... args)
    {
        return storage_.template emplace<dict_type>(std::forward<Args>(args)...);
    }

    /// @copydoc emplace_dict(Args&&... args)
    template <typename... Args>
    /// \cond CONCEPTS
/// \endcond
        requires std::constructible_from<dict_type, dict_init_list, Args...>
    /// \endcond
    dict_type& emplace_dict(dict_init_list il, Args&&... args)
    {
        return storage_.template emplace<dict_type>(std::move(il), std::forward<Args>(args)...);
    }

    /// Emplace a type T
    template <bencode_type T, typename... Args>
    /// \cond CONCEPTS
        requires std::constructible_from<bvalue_alternative_t<T, basic_bvalue>, Args...>
    /// \endcond
    decltype(auto) emplace(Args&&... args)
    {
        using Type = bvalue_alternative_t<T, basic_bvalue>;
        return storage_.template emplace<Type>(std::forward<Args>(args)...);
    }

    template <bencode_type T, typename... Args, typename = std::enable_if_t<T == bencode_type::dict>>
    /// \cond CONCEPTS
        requires std::constructible_from<dict_init_list, Args...>
    /// \endcond
    decltype(auto) emplace(dict_init_list il, Args&& ... args)
    {
        using Type = bvalue_alternative_t<T, basic_bvalue>;
        return storage_.template emplace<Type>(std::move(il), std::forward<Args>(args)...);
    }

    template <bencode_type T, typename... Args, typename = std::enable_if_t<T == bencode_type::list>>
    /// \cond CONCEPTS
        requires std::constructible_from<list_init_list, Args...>
    /// \endcond
    decltype(auto) emplace(list_init_list il, Args&& ... args)
    {
        using Type = bvalue_alternative_t<T, basic_bvalue>;
        return storage_.template emplace<Type>(std::move(il), std::forward<Args>(args)...);
    }

    template <bencode_type T, typename... Args>
    /// \cond CONCEPTS
        requires std::constructible_from<string_init_list, Args...> && (T == bencode_type::string)
    /// \endcond
    decltype(auto) emplace(string_init_list il, Args&& ... args)
    {
        using Type = bvalue_alternative_t<T, basic_bvalue>;
        return storage_.template emplace<Type>(std::move(il), std::forward<Args>(args)...);
    }

//===========================================================================//
//  Inspect current alternative                                              //
//===========================================================================//

    /// return the type as a value_type enumeration
    /// @returns bencode_type enum specifying the current active alternative
    constexpr auto type() const noexcept -> enum bencode_type
    { return static_cast<enum bencode_type>(storage_.index()); }

//===========================================================================//
//    Access current alternative                                             //
//===========================================================================//

public:
    /// @brief Returns whether the bvalue hold a value.
    /// @return Return true if the container is NOT in the unitialized state,
    ///         false otherwise.
    explicit operator bool() const noexcept
    { return (!(storage_.valueless_by_exception() || is_uninitialized())); }

//===========================================================================//
//  checked and unchecked access at()/operator[]                             //
//===========================================================================//

public:
    /// Returns a reference to the element at specified location pos, with bounds checking.
    /// If pos is not within the range of the container, an exception of type out_of_range is
    /// thrown.
    /// If the current active alternative is not a list, and exception of type
    /// bencode::bad_bvalue_access is thrown.
    /// @param pos position of the element to return
    /// @returns Reference to the requested element.
    /// @throws out_of_range if !(pos < size())
    /// @throws bad_bvalue_access if the current active alternative is not list.
    reference at(std::size_t pos)
    {
        if (!is_list())
            throw bad_bvalue_access("bvalue alternative type is not list");

        auto* l = std::get_if<list_type>(&storage_);
        if (pos >= l->size()) [[unlikely]]
            throw out_of_range(fmt::format("list index \"{}\" is out of range", pos));

        return l->operator[](pos);
    }

    /// @copydoc at(std::size_t)
    const_reference at(std::size_t pos) const
    {
        if (!is_list()) [[unlikely]]
            throw bad_bvalue_access("bvalue alternative type is not list");

        const auto* l = std::get_if<list_type>(&storage_);
        if (pos >= l->size()) [[unlikely]]
            throw out_of_range(fmt::format("list index \"{}\" is out of range", pos));

        return l->operator[](pos);
    }

    /// Returns a reference to the mapped bvalue of the element with key equivalent to key.
    /// If the current active alternative is not a dict, and exception of type
    /// bencode::bad_bvalue_access is thrown.
    /// If no such element exists, an exception of type out_of_range is thrown.
    /// @param pos  	the key of the element to find
    /// @returns Reference to the mapped bvalue of the requested element
    /// @throw out_of_range if the container does not have an element with the specified key,
    /// @throw bencode::bad_bvalue_access if the current active alternative is not dict.
    reference at(const string_type& key)
    {
        if (!is_dict()) [[unlikely]]
            throw bad_bvalue_access("bvalue alternative type is not dict");

        auto* d = std::get_if<dict_type>(&storage_);
        if (!d->contains(key)) [[unlikely]]
            throw out_of_range(fmt::format("dict key \"{}\" not found", key));

        return d->operator[](key);
    }


    /// @copydoc at(const string_type&)
    const_reference at(const string_type& key) const
    {
        if (!is_dict()) [[unlikely]]
            throw bad_bvalue_access("bvalue alternative type is not dict");
        auto* d = std::get_if<dict_type>(&storage_);
        if (auto it = d->find(key); it != d->end()) [[likely]]
            return it->second;
        throw out_of_range(fmt::format("dict key \"{}\" not found", key));
    }

    /// Return a reference to the bvalue referenced by a bpointer.
    /// @throws bpointer_error when the pointer does not resolve for this value
    const_reference at(const bpointer& pointer) const
    {
        return detail::evaluate(pointer, *this);
    }

    /// @copydoc at(const bpointer& pointer)
    reference at(const bpointer& pointer)
    {
        return detail::evaluate(pointer, *this);
    }

    /// Returns a reference to the element at specified location pos.
    /// No bounds checking is performed.
    /// If the active alternative is not a list an exception of type bencode::bad_bvalue_access is thrown.
    /// @param pos  position of the element to return
    /// @returns Reference to the requested element
    reference operator[](std::size_t pos)
    {
        if (!is_list()) [[unlikely]]
            throw bad_bvalue_access("bvalue alternative type is not list");
        return (*std::get_if<list_type>(&storage_))[pos];
    }

    /// @copydoc operator[](std::size_t pos)
    const_reference operator[](std::size_t pos) const
    {
        if (!is_list()) [[unlikely]]
            throw bad_bvalue_access("bvalue alternative type is not list");
        return (*std::get_if<list_type>(&storage_))[pos];
    }

    /// Returns a reference to the bvalue that is mapped to a key equivalent to key,
    /// performing an insertion if such key does not already exist.
    /// If the bvalue is unintialized an empty dict will be created in place.
    /// If the active alternative is not a dict an exception of type bencode::bad_bvalue_access is thrown.
    /// @param key  the key of the element to find
    /// @returns Reference to the mapped bvalue of the new element if no element with key key existed.
    ///          Otherwise a reference to the mapped bvalue of the existing element whose key is equivalent to key.
    reference operator[](const string_type& key)
    {
        if (is_uninitialized())  [[unlikely]]
            emplace_dict();
        else if (!is_dict()) [[unlikely]]
            throw bad_bvalue_access("bvalue alternative type is not dict");
        return (*std::get_if<dict_type>(&storage_))[key];

    }

    /// @copydoc operator[](const string_type&)
    reference operator[](string_type&& key)
    {
        if (is_uninitialized()) [[unlikely]]
            emplace_dict();
        else if (!is_dict()) [[unlikely]]
            throw bad_bvalue_access("bvalue alternative type is not dict");
        return (*std::get_if<dict_type>(&storage_))[std::move(key)];
    }

    /// Returns a reference to the first element in the container.
    /// Calling front on an empty container is undefined
    /// @returns reference to the first element
    /// @throws bad_bvalue_access when current active alternative is not a list.
    reference front()
    {
        if (!is_list()) [[unlikely]]
            throw bad_bvalue_access("bvalue alternative type is not list");
        return std::get_if<list_type>(&storage_)->front();
    }

    /// @copydoc front()
    const_reference front() const
    {
        if (!is_list()) [[unlikely]]
            throw bad_bvalue_access("bvalue alternative type is not list");
        return std::get_if<list_type>(&storage_)->front();
    }


    /// Returns a reference to the last element in the container.
    /// Calling back on an empty container is undefined
    /// @returns reference to the last element
    /// @throws bad_bvalue_access when current active alternative is not a list.
    reference back()
    {
        if (!is_list()) [[unlikely]]
            throw bad_bvalue_access("bvalue alternative type is not list");
        return std::get_if<list_type>(&storage_)->back();
    }

    /// @copydoc back()
    const_reference back() const
    {
        if (!is_list()) [[unlikely]]
            throw bad_bvalue_access("bvalue alternative type is not list");
        return std::get_if<list_type>(&storage_)->back();
    }

    /// Appends a new element to the end of a list.
    /// If the active alternative is uninintialized, first constructs an empty list in place.
    /// If the active alternative is not a list, an exception of type bencode::bad_bvalue_access
    /// is thrown. The arguments args... are forwarded to the constructor of the list type.
    /// @param args arguments to forward to the constructor of the element
    /// @returns Reference to the mapped bvalue of the new element if no element with key key
    ///     existed. Otherwise a reference to the mapped bvalue of the existing element whose key is
    ///     equivalent to key.
    template <typename... Args>
    reference emplace_back(Args&&... args)
    {
        if (is_uninitialized()) [[unlikely]] emplace_list();
        else if (!is_list()) [[unlikely]]
            throw bad_bvalue_access("bvalue alternative type is not list");
        return std::get_if<list_type>(&storage_)->emplace_back(std::forward<Args>(args)...);
    }

    /// Appends the given element bvalue to the end of a list.
    /// If the active alternative is uninintialized, first constructs an empty list in place.
    /// If the active alternative is not a list, an exception of type bencode::bad_bvalue_access
    /// is thrown. The arguments args... are forwarded to the constructor of the list type.
    /// @param value the bvalue of the element to append
    void push_back(const list_value_type& value)
    {
        if (is_uninitialized()) [[unlikely]] emplace_list();
        else if (!is_list()) [[unlikely]]
            throw bad_bvalue_access("bvalue alternative type is not list");
        auto* ptr = std::get_if<list_type>(&storage_);
        ptr->push_back(value);
    }

    /// @copydoc push_back(const list_value_type&)
    void push_back(list_value_type&& value)
    {
        if (is_uninitialized()) [[unlikely]] emplace_list();
        else if (!is_list()) [[unlikely]]
            throw bad_bvalue_access("bvalue alternative type is not list");
        auto* ptr = std::get_if<list_type>(&storage_);
        ptr->push_back(std::move(value));
    }

    /// Checks if there is an element with key equivalent to key in the container.
    /// If the active alternative is not a dict, an exception of type bad_bvalue_access is thrown.
    /// @param key 	key bvalue of the element to search for
    /// @returns true if there is such an element, otherwise false.
    bool contains(const dict_key_type& key) const
    {
        if (!is_dict())
            throw bad_bvalue_access("bvalue alternative type is not dict");
        auto& bdict = *std::get_if<dict_type>(&storage_);
        return bdict.find(key) != end(bdict);
    }

    /// Checks if there is an element that compares equivalent to key in the container.
    /// If the active alternative is not a dict, an exception of type bad_bvalue_access is thrown.
    /// This overload only participates in overload resolution if the
    /// map_type key_compare function supports transparent comparison.
    /// @param key 	key bvalue of the element to search for
    /// @returns true if there is such an element, otherwise false.
    template <typename K>
    /// \cond CONCEPTS
        requires std::totally_ordered_with<K, dict_key_type> &&
                detail::policy_dict_key_compare<Policy>::is_transparent
    /// \endcond
    bool contains(const K& key) const
    {
        if (!is_dict())
            throw bad_bvalue_access("bvalue alternative type is not dict");
        auto& bdict = *std::get_if<dict_type>(&storage_);
        return bdict.find(key) != end(bdict);
    }


    /// Return a reference to the bvalue referenced by a bpointer.
    /// @throws bpointer_error when the pointer does not resolve for this value
    bool contains(const bpointer& pointer) const
    {
        return detail::contains(pointer, *this);
    }

    /// Remove all elements from the current alternative.
    /// If the current alternative is a dict, list, string, calls clear on the underlying container.
    /// If the current alternative is an integer, set the bvalue to zero.
    /// If the current alternative is uninitialized, do nothing.
    void clear() noexcept
    {
        std::visit([](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, uninitialized_type>)
                return;
            else if constexpr (std::is_same_v<T, integer_type>)
                arg = 0;
            else
                arg.clear();
        }, storage_);
    }

public:
    void swap(basic_bvalue& other)
    noexcept(noexcept(std::declval<storage_type>().swap(std::declval<storage_type>())))
    { storage_.swap(other.storage_); }

    /// Compares the contents of two basic_bvalue types.
    /// @param that the basic_bvalue whose content to compare
    /// @returns true of the lhs and rhs compare equal, false otherwise.
    constexpr bool operator==(const basic_bvalue& that) const noexcept
    {
        if (this->type() == that.type()) {
            auto r = (this->storage_ == that.storage_);
            return r;
        } else {
            return this->type() == that.type();
        }
    }

    /// Compares the content of the current alternative with the content of that
    /// lexicographically.
    /// @param that the basic_bvalue whose content to compare
    /// @returns
    ///     std::partial_ordering::unordered if the current alternatives are of different types,
    ///     otherwise return the result of the comparison as a std::weak_ordering.
    constexpr std::weak_ordering operator<=>(const basic_bvalue& that) const noexcept
    {
        if (this->type() == that.type()) {
            return std::compare_weak_order_fallback(this->storage_, that.storage_);
        }
        else {
            return this->type() <=> that.type();
        }
    }

    /// Compares the current alternatives content with that.
    /// @param that the bvalue to compare to.
    /// @returns true of the lhs and rhs compare equal, false otherwise.
    template <typename T>
    /// \cond CONCEPTS
        requires (!basic_bvalue_instantiation<T>)
    /// \endcond
    constexpr auto operator==(const T& that) const noexcept -> bool
    {
        return detail::compare_equality_with_bvalue(*this, that);
    }

    /// Compares the current alternatives content with that.
    /// @param that the basic_bvalue whose content to compare
    /// @returns
    ///     std::partial_ordering::unordered if the current alternatives are of different types,
    ///     otherwise return the result of the comparison as a std::weak_ordering.
    template <typename T>
    /// \cond CONCEPTS
        requires (!basic_bvalue_instantiation<T>)
    /// \endcond
    constexpr auto operator<=>(const T& that) const noexcept -> std::weak_ordering
    {
        return detail::compare_three_way_with_bvalue(*this, that);
    }

private:
    template <typename T, class... Args >
    constexpr explicit basic_bvalue(std::in_place_type_t<T>,
                                   Args&&... args)
            : storage_(std::in_place_type<T>, std::forward<Args>(args)...)
    {}

    template <std::size_t I, class... Args >
    constexpr explicit basic_bvalue(std::in_place_index_t<I>,
                                   Args&&... args)
            : storage_(std::in_place_index<I>, std::forward<Args>(args)...)
    {}

    template <typename T, typename U, class... Args >
    constexpr explicit basic_bvalue(std::in_place_type_t<T>,
                                   std::initializer_list<U> il,
                                   Args&&... args)
            : storage_(std::in_place_type<T>, std::move(il), std::forward<Args>(args)...)
    {}

    template <std::size_t I, typename U, class... Args >
    constexpr explicit basic_bvalue(std::in_place_index_t<I>,
                                   std::initializer_list<U> il,
                                   Args&&... args)
            : storage_(std::in_place_index<I>, std::move(il), std::forward<Args>(args)...)
    {}

    #define BENCODE_IS_ALTERNATIVE_TYPE_FUNCTION_TEMPLATE(ALTERNATIVE_TYPE)       \
    constexpr bool is_##ALTERNATIVE_TYPE() const noexcept                     \
    { return std::holds_alternative<ALTERNATIVE_TYPE##_type>(storage_); }     \

    BENCODE_IS_ALTERNATIVE_TYPE_FUNCTION_TEMPLATE(uninitialized)
    BENCODE_IS_ALTERNATIVE_TYPE_FUNCTION_TEMPLATE(integer)
    BENCODE_IS_ALTERNATIVE_TYPE_FUNCTION_TEMPLATE(string)
    BENCODE_IS_ALTERNATIVE_TYPE_FUNCTION_TEMPLATE(list)
    BENCODE_IS_ALTERNATIVE_TYPE_FUNCTION_TEMPLATE(dict)

    #undef BENCODE_IS_ALTERNATIVE_TYPE_FUNCTION_TEMPLATE

    constexpr bool is_primitive() const noexcept
    { return !is_structured(); }

    constexpr bool is_structured() const noexcept
    { return is_list() || is_dict(); }

    template <typename P>
    friend constexpr const auto& detail::get_storage(const basic_bvalue<P>& value) noexcept;

    template <typename P>
    friend constexpr auto& detail::get_storage(basic_bvalue<P>& value) noexcept;

    template <typename P>
    friend constexpr const auto* detail::get_storage(const basic_bvalue<P>* value) noexcept;

    template <typename P>
    friend constexpr auto* detail::get_storage(basic_bvalue<P>* value) noexcept;

    storage_type storage_;
};


template <typename Policy>
struct serialization_traits<basic_bvalue<Policy>> : serializes_to_runtime_type {};


//============================================================================//
//  Helper classes                                                            //
//============================================================================//

/// Alias for basic_bvalue<default_bvalue_policy>
using bvalue = basic_bvalue<default_bvalue_policy>;

} // namespace bencode
