#pragma once

#include <compare>
#include <cassert>

#include "bencode/bencode_fwd.hpp"
#include "bencode/detail/bview/concepts.hpp"
#include "bencode/detail/descriptor.hpp"
#include "bencode/detail/bview/bad_bview_access.hpp"


namespace bencode {

// forward declaration
class bview;


namespace detail {

constexpr const descriptor* get_storage(const bview& value) noexcept;

template <serializable T>
constexpr bool compare_equality_with_bview(const bview& bv, const T& value);

template <serializable T>
constexpr std::weak_ordering compare_three_way_with_bview(const bview& bv, const T& value);

constexpr bool compare_equality_bview_impl(const bview& lhs, const bview& rhs) noexcept;

constexpr std::weak_ordering compare_three_way_bview_impl(const bview& lhs, const bview& rhs) noexcept;

} // namespace detail


/// The class bview provides read-only access to a bencoded value.
/// The implementation holds only two members: a pointer pointing into contiguous sequence of descriptors
/// and pointer to the buffer containing the bencoded data.
/// Descriptors provide the structural information necessary to navigate the bencoded data.
/// The bview class and it's subclasses provide a convenient interface to interact with the bencoded data.
///
/// The bview class describes bencoded data of unknown type. The type can be queried by type().
/// bview subclasses describe bencoded data with a known type and can thus provide a richer interface.
class bview {
public:
    // storage type aliasesPolicy
    using integer_type       = integer_bview;
    using string_type        = string_bview;
    using list_type          = list_bview;
    using dict_type          = dict_bview;

    /// Default constructor. Constructs an empty bview.
    /// After construction, data() is equal to nullptr, and size() is equal to 0.
    constexpr bview() noexcept
        :  buffer_(nullptr), desc_(nullptr)
    {}

    /// Constructs a bview from a descriptor and a character buffer.
    /// Behavior is undefined when data or buffer is nullptr
    constexpr bview(const descriptor* data, const char* buffer) noexcept
            : buffer_(buffer)
            , desc_(data)
    {
        Expects(data != nullptr);
        Expects(buffer != nullptr);
    }

    /// Copy constructor.
    constexpr bview(const bview&) noexcept = default;

    /// Copy assignment.
    constexpr bview& operator=(const bview&) noexcept = default;

    explicit constexpr operator bool() const noexcept
    { return desc_ != nullptr && buffer_ != nullptr; }

    /// Returns the type of the descriptor this bview points to.
    /// @returns The bencode data type described by this bview.
    constexpr bencode::bencode_type type() const noexcept
    {
        if (desc_ == nullptr)          return bencode_type::uninitialized;
        if (desc_->is_integer())       return bencode_type::integer;
        if (desc_->is_string())        return bencode_type::string;
        if (desc_->is_list())          return bencode_type::list;
        if (desc_->is_dict())          return bencode_type::dict;
        BENCODE_UNREACHABLE;
    }
    
    /// Return the bencoded representation of the bvalue.
    /// @returns Bencoded representation of the described value.
    constexpr std::string_view bencoded_view() const noexcept
    {
        if (desc_->is_integer())
            return {buffer_, detail::base_ten_digits(desc_->value()) + 2 };
        else if (desc_->is_string())
            return {buffer_, desc_->offset() + desc_->size()};
        else {
            auto end_desc = std::next(desc_, desc_->offset());
            return {buffer_, end_desc->position() + 1 - desc_->position()};
        }
    }

    constexpr bool operator==(const bview& that) const noexcept
    {
        // defined in compare in avoid circular dependency with accessors
        return detail::compare_equality_bview_impl(*this, that);
    }

    constexpr std::weak_ordering operator<=>(const bview& that) const noexcept
    {
        // defined in compare in avoid circular dependency with accessors
        return detail::compare_three_way_bview_impl(*this, that);
    }

    /// Compares the current alternatives content with that.
    /// @param that the bvalue to compare to.
    /// @returns true of the lhs and rhs compare equal, false otherwise.
    template <typename T>
    /// \cond CONCEPTS
        requires (!std::same_as<T, bview> && !bview_alternative_type<T>)
    /// \endcond
    constexpr auto operator==(const T& that) const noexcept -> bool
    {
        return detail::compare_equality_with_bview(*this, that);
    }

    /// Compares the current alternatives content with that.
    /// @param that the bview whose content to compare
    /// @returns
    ///     std::partial_ordering::unordered if the current alternatives are of different types,
    ///     otherwise return the result of the comparison as a std::weak_ordering.
    template <typename T>
    /// \cond CONCEPTS
        requires (!std::same_as<T, bview> && !bview_alternative_type<T>)
    /// \endcond
    constexpr std::weak_ordering operator<=>(const T& that) const noexcept
    {
        return detail::compare_three_way_with_bview(*this, that);
    }

    /// Returns a view to the element at specified location pos.
    /// No bounds checking is performed.
    /// Out of bounds access results in undefined behavior.
    /// If the active alternative is not a list an exception of type bad_bview_access is thrown.
    /// @param pos  position of the element to return
    /// @returns view to the requested element.
    constexpr bview operator[](std::size_t pos) const;


    /// Returns a view to the element at specified location pos, with bounds checking.
    /// If pos is not within the range of the list, an exception of type out_of_range is
    /// thrown.
    /// If the current active alternative is not a list, and exception of type
    /// bad_bview_access is thrown.
    /// @param pos position of the element to return
    /// @returns View of the requested element.
    /// @throws out_of_range if !(pos < size())
    /// @throws bad_bview_access if the current active alternative is not a list.
    constexpr bview at(std::size_t pos) const;

    /// Returns a reference to the mapped bvalue of the element with key equivalent to key.
    /// If the current active alternative is not a dict, and exception of type
    /// bad_bview_access is thrown.
    /// If no such element exists, an exception of type std::out_of_range is thrown.
    /// @param key the key of the element to find
    /// @returns View to the mapped value of the requested element.
    /// @throws out_of_range if the container does not have an element with the specified key,
    /// @throws bad_bview_access if the current active alternative is not dict.
    constexpr bview at(std::string_view key) const;

    /// Return a view to the value referenced by a bpointer.
    /// If the bpointer does not resolve an exceptionn of type out_of_range is thrown.
    /// @param pointer the bpointer to the element to return
    /// @returns View to the element pointed to by pointer.
    /// @throws out_of_range if the pointer does not resolve for this value
    bview at(const bpointer& pointer) const;

    /// Returns a view to the first element in the list.
    /// Calling front on an empty container is undefined behavior.
    /// @returns reference to the first element
    /// @throws bad_bview_access when the current active alternative is not a list.
    constexpr bview front() const;

    /// Returns a view to the last element in the list.
    /// Calling back on an empty list is undefined behavior.
    /// @returns reference to the last element
    /// @throws bad_bview_access when current active alternative is not a list.
    constexpr bview back() const;

    /// Checks if there is an element with key equivalent to key in the dict.
    /// If the active alternative is not a dict, an exception of type bad_bview_access is thrown.
    /// @param key 	key bvalue of the element to search for
    /// @returns true if there is such an element, otherwise false.
    constexpr bool contains(std::string_view key) const;

    bool contains(const bpointer& pointer) const;

protected:
    constexpr bool is_integer() const noexcept
    { return desc_->is_integer(); }

    constexpr bool is_string() const noexcept
    { return desc_->is_string(); }

    constexpr bool is_list() const noexcept
    { return desc_->is_list(); }

    constexpr bool is_dict() const noexcept
    { return desc_->is_dict(); }

    friend constexpr const descriptor* detail::get_storage(const bview& value) noexcept;

    /// pointer to bencode data
    const char* buffer_;
    /// pointer into contiguous sequence of descriptors
    const descriptor* desc_;
};

} // namespace bencode

BENCODE_SERIALIZES_TO_RUNTIME_TYPE(bencode::bview);


