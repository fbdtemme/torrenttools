#pragma once

#include <compare>
#include <concepts>
#include <ranges>
#include <string_view>
#include <iterator>
#include <numeric>
#include <algorithm>

#include <gsl-lite/gsl-lite.hpp>

#include "bencode/detail/bview/bview.hpp"

namespace bencode {

namespace rng = std::ranges;

/// A view into a bencoded list that provides access to the data similar to a std::string_view.
class string_bview : public bview
{
public:
    using value_type = char;
    using const_pointer = const char*;
    using const_reference = const char&;
    using const_iterator = const char*;
    using const_reverse_iterator = std::reverse_iterator<const char*>;
    using size_type = std::size_t;
    static constexpr size_type npos = -1;

    /// Default constructor. Constructs an empty string_bview.
    /// After construction, data() is equal to nullptr, and size() is equal to 0.
    constexpr explicit string_bview() noexcept
            :bview() {}

    /// Downcasting constructor. Constructs a string_bview from a bview.
    /// Behavior is undefined when the bview type is not a string.
    constexpr explicit string_bview(const bview& desc) noexcept
            :bview(desc)
    {
        Expects(desc_->is_string());
    }

    /// Constructs a string_bview from a descriptor and a character buffer.
    /// Behavior is undefined when the descriptor type is not a string.
    constexpr string_bview(const descriptor* data, const char* buffer) noexcept
            :bview(data, buffer)
    {
        Expects(desc_->is_string());
    }

    /// Copy constructor.
    constexpr string_bview(const string_bview&) noexcept = default;

    /// Copy assignment.
    constexpr string_bview& operator=(const string_bview&) noexcept = default;

    // Iterators

    /// Returns an iterator to the first character of the view.
    /// @returns const_iterator to the first character
    constexpr const_iterator begin() const noexcept
    { return buffer_ + desc_->position() + desc_->offset(); }

    /// @copydoc begin()
    constexpr const_iterator cbegin() const noexcept
    { return begin(); }

    /// Returns an iterator to the character following the last character.
    /// This element acts as a placeholder; attempting to access it results in undefined behavior.
    /// @returns const_iterator to the character following the last character.
    constexpr const_iterator end() const noexcept
    { return buffer_ + desc_->position() + desc_->offset() + desc_->size(); }

    /// @copydoc end()
    constexpr const_iterator cend() const noexcept
    { return end(); }

    /// Returns a reverse iterator to the first character of the reversed view.
    /// It corresponds to the last character of the non-reversed view.
    /// @returns const_reverse_iterator to the first character
    constexpr const_reverse_iterator rbegin() const noexcept
    { return std::reverse_iterator(end()); }

    /// @copydoc rbegin()
    constexpr const_reverse_iterator crbegin() const noexcept
    { return std::reverse_iterator(end()); }

    /// Returns a reverse iterator to the character following the last character of the reversed view.
    /// It corresponds to the character preceding the first character of the non-reversed view.
    /// This character acts as a placeholder, attempting to access it results in undefined behavior.
    /// @returns const_reverse_iterator to the character following the last character.
    constexpr const_reverse_iterator rend() const noexcept
    { return std::reverse_iterator(begin()); }

    /// @copydoc rend()
    constexpr const_reverse_iterator crend() const noexcept
    { return std::reverse_iterator(begin()); }
    // Element access

    /// Returns a const reference to the character at specified location pos.
    /// No bounds checking is performed: the behavior is undefined if pos >= size().
    /// @param pos position of the character to return
    /// @returns Const reference to the requested character.
    constexpr const_reference operator[](size_type pos) const
    { return *(data() + pos); }

    /// Returns a const reference to the character at specified location pos.
    /// Bounds checking is performed, exception of type std::out_of_range will be thrown on
    /// invalid access.
    constexpr const_reference at(size_type pos) const
    {
        if (pos >= size()) [[unlikely]] throw std::out_of_range("invalid pos");
        return this->operator[](pos);
    }

    /// Returns reference to the first character in the view.
    /// The behavior is undefined if empty() == true.
    /// @returns Reference to the first character, equivalent to operator[](0).
    constexpr const_reference front() const
    { return this->operator[](0); }

    /// Returns reference to the last character in the view.
    /// The behavior is undefined if empty() == true.
    /// @returns Reference to the last character, equivalent to operator[](size() - 1).
    constexpr const_reference back() const
    { return this->operator[](size() - 1); }

    /// Returns a pointer to the underlying character array.
    /// The pointer is such that the range [data(); data() + size()) is valid and the values in
    /// it correspond to the values of the view.
    /// @returns A pointer to the underlying character array.
    constexpr const_pointer data() const noexcept
    { return buffer_ + desc_->position() + desc_->offset(); }

    // capacity

    /// Returns the number of CharT elements in the view, i.e. std::distance(begin(), end()).
    /// @returns The number of CharT elements in the view.
    constexpr size_type size() const noexcept
    { return desc_->size(); }

    /// @copydoc size()
    constexpr size_type length() const noexcept
    { return size(); }

    /// The largest possible number of char-like objects that can be referred to by a string_bview.
    /// @returns Maximum number of characters.
    constexpr size_type max_size() const noexcept
    { return std::numeric_limits<decltype(desc_->size())>::max(); }

    /// Checks if the view has no characters, i.e. whether size() == 0.
    /// @returns true if the view is empty, false otherwise
    [[nodiscard]] constexpr bool empty() const noexcept
    { return desc_ == nullptr || size() == 0; }

    // Operations

    /// Returns a std::string_View of the substring [pos, pos + rcount),
    /// where rcount is the smaller of count and size() - pos.
    /// @param pos	position of the first character
    /// @param count requested length
    /// @returns View of the substring [pos, pos + rcount).
    /// @throws std::out_of_range if pos > size()
    constexpr std::string_view substr(size_type pos = 0, size_type count = npos ) const
    {
        if (count == npos) return std::string_view(data() + pos, size()-pos);
        return std::string_view(data() + pos, count);
    }

    /// Checks if the string view begins with the given prefix.
    /// @param sv a string view which may be a result of implicit conversion from std::basic_string
    /// @returns true if the string view begins with the provided prefix, false otherwise.
    constexpr bool starts_with(std::string_view sv) const noexcept
    { return substr(0, sv.size()) == sv; }

    /// Checks if the string view begins with the given character.
    /// @param c a single character
    /// @returns true if the string view begins with the provided prefix, false otherwise.
    constexpr bool starts_with(char c) const noexcept
    { return !empty() && front() == c; }

    /// Checks if the string view begins with the given prefix.
    /// @param s a null-terminated character string
    /// @returns true if the string view begins with the provided prefix, false otherwise.
    constexpr bool starts_with(const char* s) const
    { return starts_with(std::string_view(s)); }

    /// Checks if the string view ends with the given prefix.
    /// @param sv a string view which may be a result of implicit conversion from std::basic_string
    /// @returns true if the string view ends with the provided prefix, false otherwise.
    constexpr bool ends_with(std::string_view sv) const noexcept
    { return size() >= sv.size() && substr(size() - sv.size(), npos) == sv; }

    /// Checks if the string view ends with the given character.
    /// @param c a single character
    /// @returns true if the string view ends with the provided prefix, false otherwise.
    constexpr bool ends_with(char c) const noexcept
    { return !empty() && back() == c; }

    /// Checks if the string view ends with the given prefix.
    /// @param s a null-terminated character string
    /// @returns true if the string view ends with the provided prefix, false otherwise.
    constexpr bool ends_with(const char* s) const
    { return ends_with(std::string_view(s)); }

    /// Implicit conversion operator to std::string_view.
    constexpr operator std::string_view() const noexcept
    { return std::string_view(begin(), end()); }

    /// Compare equality with a bview.
    ///@param rhs a bvalue to compare
    constexpr bool operator==(const string_bview& rhs) const noexcept {
        return std::equal(begin(), end(), rhs.begin(), rhs.end());
    }

    /// Compare order with a bview.
    ///@param rhs a bvalue to compare
    constexpr std::strong_ordering operator<=>(const string_bview& rhs) const noexcept {
        return std::lexicographical_compare_three_way(begin(), end(), rhs.begin(), rhs.end());
    }

    /// Compare equality with a std::string_view.
    /// @param rhs a string_view to compare
    /// @note this overload is used to compare with std::string and c-strings as well.
    constexpr bool operator==(std::string_view rhs) const noexcept {
        auto lhs = std::string_view(*this);
        return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
    }

    /// Compare order with a std::string_view.
    /// @param rhs the value to compare with
    /// @note this overload is used to compare with std::string and c-strings as well.
    constexpr std::strong_ordering operator<=>(std::string_view rhs) const noexcept {
        auto lhs = std::string_view(*this);
        return std::lexicographical_compare_three_way(
                lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
    }
};

static_assert(rng::contiguous_range<string_bview>);
static_assert(std::three_way_comparable<string_bview>);
static_assert(std::equality_comparable<string_bview>);



} // namespace bencode

BENCODE_SERIALIZES_TO_STRING(bencode::string_bview);
