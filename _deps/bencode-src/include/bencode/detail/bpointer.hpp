#pragma once
#include <vector>
#include <string>
#include <string_view>
#include <stdexcept>
#include <algorithm>
#include <compare>
#include <gsl-lite/gsl-lite.hpp>

#include <fmt/format.h>

#include "bencode/detail/parser/from_chars.hpp"
#include "bencode/detail/concepts.hpp"
#include "bencode/detail/exception.hpp"


namespace bencode {

namespace rng = std::ranges;
using namespace std::string_view_literals;

class bpointer_error : public exception
{
public:
    using exception::exception;
};


/// A class representing a bencode pointer.

/// Bencode pointers are based on JSON pointers (RFC6901).
/// A bencode pointer is a string representation to identify specific values in a bencode data
/// structure.
/// Syntax is the same as json pointers:
///     * Dict keys and array indices are seperated by '/'.
///     * '~' in a dict key is escaped to '~0'.
///     * '/' in a dict key is escaped to '~1'.
///     * leading zeros in a list index is not allowed.
class bpointer
{
    using storage_type = std::vector<std::string>;
public:
    using iterator               = storage_type::iterator;
    using const_iterator         = storage_type::const_iterator;
    using reverse_iterator       = storage_type::reverse_iterator;
    using const_reverse_iterator = storage_type::const_reverse_iterator;

    /// Construct an empty bpointer
    bpointer() noexcept = default;

    /// Construct a bpointer from the string representation.
    explicit bpointer(std::string_view expression)
        : tokens_(split_tokens(expression))
    {}

    /// Construct a bpointer from an initialized list of reference tokens.
    bpointer(std::initializer_list<std::string> il)
        : tokens_(il)
    {}

    // iterators

    const_iterator begin() const noexcept
    { return tokens_.begin(); }

    iterator begin() noexcept
    { return tokens_.begin(); }

    const_iterator cbegin() const noexcept
    { return tokens_.begin(); }

    const_iterator end() const noexcept
    { return tokens_.end(); }

    iterator end() noexcept
    { return tokens_.end(); }

    const_iterator cend() const noexcept
    { return tokens_.end(); }

    const_reverse_iterator rbegin() const noexcept
    { return tokens_.rbegin(); }

    reverse_iterator rbegin() noexcept
    { return tokens_.rbegin(); }

    const_reverse_iterator crbegin() const noexcept
    { return tokens_.rbegin(); }

    const_reverse_iterator rend() const noexcept
    { return tokens_.rend(); }

    reverse_iterator rend() noexcept
    { return tokens_.rend(); }

    const_reverse_iterator crend() const noexcept
    { return tokens_.rend(); }

    /// Compare a bpointer for equality.
    /// @returns true if equal, false otherwise
    bool operator==(const bpointer& rhs) const noexcept
    {
        return this->tokens_ == rhs.tokens_;
    }

    /// Compare a bpointer with the string representation of a bpointer.
    /// @returns true if equal, false otherwise
    bool operator==(std::string_view rhs) const noexcept
    {
        return to_string() == rhs;
    }


    /// Compare the order of a bpointer.
    /// The order is defined as the lexicographical order of the string representation.
    std::strong_ordering operator<=>(const bpointer& rhs) const noexcept
    {
        return std::compare_strong_order_fallback(this->tokens_, rhs.tokens_);
    }

    // Appending

    /// Append a bpointer to the current pointer.
    /// @param ptr the bpointer to append
    /// @returns a reference to the bpointer
    bpointer& append(const bpointer& ptr)
    {
        tokens_.insert(tokens_.end(), ptr.tokens_.begin(), ptr.tokens_.end());
        return *this;
    }

    /// Append an unescaped reference token to the current pointer.
    /// @param dict_key the reference token to append.
    /// @returns a reference to the bpointer
    bpointer& append(std::string_view dict_key)
    {
        tokens_.emplace_back(dict_key);
        return *this;
    }


    /// Append a list index to the current bpointer.
    /// @param list_index the list index to append.
    /// @returns a reference to the bpointer
    bpointer& append(std::size_t list_index)
    {
        tokens_.emplace_back(std::to_string(list_index));
        return *this;
    }

    /// @copydoc append(const bpointer&)
    bpointer& operator/=(const bpointer& ptr)
    {
        return append(ptr);
    }

    /// @copydoc append(std::string_view)
    bpointer& operator/=(std::string_view token)
    {
        return append(token);
    }

    /// @copydoc append(std::size_t)
    bpointer& operator/=(std::size_t list_index)
    {
        return append(list_index);
    }


    /// Concatenate two bpointers.
    /// @param lhs, rhs the two pointers to concatenate
    /// @returns a new bpointer constructed from the concatenation of lhs and rhs
    friend bpointer operator/(const bpointer& lhs, const bpointer& rhs)
    {
        return bpointer(lhs).append(rhs);
    }

    /// Concatenate a bpointer and an unescaped reference token.
    /// @param ptr the bpointer to concatenate with
    /// @paran token the refence token to concatenate to the bpointer
    /// @returns a new bpointer constructed from the concatenation of ptr and token
    friend bpointer operator/(const bpointer& ptr, std::string_view token)
    {
        return bpointer(ptr).append(token);
    }

    /// Concatenate a bpointer and an array index.
    /// @param ptr the bpointer to concatenate with
    /// @paran idx the list index to concatenate to the bpointer
    /// @returns a new bpointer constructed from the concatenation of ptr and idx
    friend bpointer operator/(const bpointer& ptr, std::size_t idx)
    {
        return bpointer(ptr).append(idx);
    }

    /// Return a string representation of the bpointer.
    std::string to_string() const
    {
        std::string result {};
        rng::for_each(tokens_, [&](const auto& token) {
            result.push_back('/');
            result.append(escape(token));
        });
        return result;
    }

    /// @copydoc to_string()
    explicit operator std::string() const
    { return to_string(); }

    /// Return the parent of this bpointer.
    /// @returns the parent of this bpointer,
    ///          if the pointer is the root, the root itself is returned
    bpointer parent() const
    {
        if (tokens_.empty()) {
            return *this;
        }

        bpointer res {};
        res.tokens_.assign(tokens_.begin(), tokens_.end()-1);
        return res;
    }

    /// Returns if the bpointer points to the root document.
    /// @returns true if the pointer pointer to the document root, false otherwise.
    bool empty() const noexcept
    {
        return tokens_.empty();
    }

    /// Return a reference to the first reference token.
    /// The behavior is undefined if empty() == true.
    /// @returns reference to the first reference token.
    const std::string& front() const
    {
        return tokens_.front();
    }

    /// @copydoc front()
    std::string& front()
    {
        return tokens_.front();
    }

    /// Return a reference to the last reference token.
    /// The behavior is undefined if empty() == true.
    /// @returns a reference to the last reference token
    const std::string& back() const
    {
        return tokens_.back();
    }

    /// @copydoc back()
    std::string& back()
    {
        return tokens_.back();
    }

private:
    template <rng::input_range Rng, std::output_iterator<char> OutputIt>
        requires std::same_as<rng::range_value_t<Rng>, char>
    static void escape(Rng&& token, OutputIt out)
    {
        auto result = std::string(token);
        auto prev = rng::begin(token);
        auto last = rng::end(token);
        auto it = rng::find_first_of(rng::subrange(prev, last), "~/");
        for ( ; it != last ; prev = ++it, it = rng::find_first_of(rng::subrange(prev, last), "~/"))
        {
            rng::copy(prev, it, out);
            auto c = *it;
            if (c == '~')
                rng::copy("~0"sv, out);
            else if (c == '/')
                rng::copy("~1"sv, out);
            else
                Ensures(false);
        }
        rng::copy(prev, last, out);
    }

    template <rng::input_range Rng>
        requires std::same_as<rng::range_value_t<Rng>, char>
    static std::string escape(Rng&& token)
    {
        std::string result;
        escape(std::forward<Rng>(token), std::back_inserter(result));
        return result;
    }

    template <rng::input_range Rng, std::output_iterator<char> OutputIt>
        requires std::same_as<rng::range_value_t<Rng>, char>
    static void unescape(Rng&& token, OutputIt out)
    {
        auto end = rng::end(token);
        auto prev = rng::begin(token);
        auto cur = rng::find(prev, end, '~');
        for ( ; cur != rng::end(token); prev = cur, cur = rng::find(prev, end, '~'))
        {
            // copy all characters up until the next escape character
            rng::copy(prev, cur, out);

            auto next_char = *++cur;
            if (next_char == '0')
                *out++= '~';
            else if (next_char == '1')
                *out++= '/';
            else
                throw bpointer_error("escape character '~' must be followed with '0' or '1'");
            // move past 0 or 1
            ++cur;
        }
        // copy all characters from last ~ to end
        rng::copy(prev, end, out);
    }

    template <rng::input_range Rng>
        requires std::same_as<rng::range_value_t<Rng>, char>
    static std::string unescape(Rng&& token)
    {
        std::string result;
        unescape(std::forward<Rng>(token), std::back_inserter(result));
        return result;
    }

    template <rng::input_range Rng>
        requires std::same_as<rng::range_value_t<Rng>, char>
    std::vector<std::string> split_tokens(Rng&& reference)
    {
        std::vector<std::string> result {};

        if (rng::empty(reference)) {
            return result;
        }

        if (*rng::begin(reference) != '/') [[unlikely]] {
            throw bencode::bpointer_error("bencode pointer must be empty or begin with '/'");
        }

        auto prev = std::next(rng::begin(reference));
        auto last = rng::end(reference);
        auto cur = std::find(prev, last, '/');
        for ( ; cur != last; prev = std::next(cur), cur = std::find(prev, last, '/'))
        {
           auto token = std::string(prev, cur);
           token = unescape(token);
           result.push_back(std::move(token));
        }
        result.push_back(unescape(std::string(prev, last)));
        return result;
    }

//    template <bvalue_or_bview T> friend const T& evaluate(const bpointer& pointer, const T& bv);

    std::vector<std::string> tokens_;
};

namespace literals {

inline bpointer operator ""_bpointer(const char* data, const std::size_t size)
{
    return bpointer(std::string_view{data, size});
}

}

} // namespace bencode
