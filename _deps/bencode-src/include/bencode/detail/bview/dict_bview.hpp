#pragma once

#include <algorithm>
#include <iterator>
#include <utility>
#include <gsl-lite/gsl-lite.hpp>

#include <compare>

#include "bencode/detail/symbol.hpp"
#include "bencode/detail/bview/bview.hpp"
#include "bencode/detail/bview/string_bview.hpp"

namespace bencode {


namespace detail {

class dict_bview_iterator
{
public:
    using iterator_type = dict_bview_iterator;
    using iterator_category = std::bidirectional_iterator_tag;
    using key_type = string_bview;
    using mapped_type = bview;
    using value_type = const std::pair<string_bview, bview>;
    using difference_type = std::ptrdiff_t;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using reference = value_type&;
    using const_reference = const value_type&;


    constexpr dict_bview_iterator() noexcept
            : data_(nullptr), buffer_(nullptr), begin_(nullptr) {};

    constexpr dict_bview_iterator(const dict_bview_iterator&) noexcept = default;

    constexpr dict_bview_iterator(const descriptor* data, const char* buffer) noexcept
            : data_(data)
            , buffer_(buffer)
            , begin_(data)
    {
        Expects(data_ != nullptr);
        Expects(buffer_ != nullptr);
    }

    constexpr auto operator*() const noexcept -> const_reference
    {
        Expects(!is_end_iterator());
        dereference_buffer_ = {key(), value()};
        return dereference_buffer_;
    }

    constexpr auto operator->() const noexcept -> const_pointer
    {
        Expects(!is_end_iterator());
        dereference_buffer_ = {key(), value()};
        return &dereference_buffer_;
    }

    constexpr auto key() const -> key_type
    {
        Expects(!is_end_iterator());
        return {data_, buffer_};
    }

    constexpr auto value() const -> mapped_type
    {
        Expects(!is_end_iterator());
        return {std::next(data_), buffer_};
    }

    constexpr auto operator++() -> iterator_type&
    {
        Expects(!is_end_iterator());
        std::advance(data_, 1);
        // skip to end token of nested structures
        if (data_->is_list_begin() || data_->is_dict_begin()) {
            std::advance(data_, data_->offset());
        }
        std::advance(data_, 1);
        return *this;
    }

    constexpr auto operator++(int) -> iterator_type
    {
        auto copy = *this;
        operator++();
        return copy;
    }

    constexpr auto operator--() -> iterator_type&
    {
        // go to previous value
        std::advance(data_, -1);
        // check if the value is nested
        if (data_->is_list_end() || data_->is_dict_end()) {
            std::advance(data_, -int(data_->offset()));
        }
        std::advance(data_, -1);
        return *this;
    }

    constexpr auto operator--(int) -> iterator_type
    {
        auto copy = *this;
        operator--();
        return copy;
    }

    constexpr bool operator==(const iterator_type& that) const noexcept
    {
        return buffer_ == that.buffer_ && data_ == that.data_;
    }

private:
    constexpr bool is_end_iterator() const noexcept
    { return data_->is_dict_end() && (data_ - data_->offset())->is_dict_begin(); }

    const descriptor* data_;
    const char* buffer_;
    mutable std::remove_const_t<value_type> dereference_buffer_;
    const descriptor* begin_;
};
}

/// A view into a bencoded dict that provides access to the elements similar to a std::map.
/// @note dereferencing an iterator return a reference to a std::pair<string_bview, bview>
///       but this reference is invalidated upon the next dereference.
class dict_bview : public bview
{
public:
    using key_type = string_bview;
    using mapped_type = bview;
    using value_type = const std::pair< key_type, mapped_type>;
    using const_reference = const value_type&;
    using const_iterator = detail::dict_bview_iterator;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    using size_type = std::size_t;

    constexpr dict_bview() noexcept = default;

    /// Converting constructor from bview.
    explicit constexpr dict_bview(const bview& bref) noexcept
            : bview(bref)
    {
        Expects(desc_->is_dict());
    }

    constexpr dict_bview(const descriptor* data, const char* buffer) noexcept
            :bview(data, buffer)
    {
        Expects(desc_->is_dict());
    }

    /// Copy constructor.
    constexpr dict_bview(const dict_bview&) noexcept = default;

    /// Copy assignment.
    constexpr dict_bview& operator=(const dict_bview& view) noexcept = default;

    // element access

    /// Returns a reference to the mapped value of the element with key equivalent to key.
    /// If no such element exists, an exception of type std::out_of_range is thrown.
    /// @param key the key of the element to find
    /// @returns Reference to the mapped value of the requested element.
    constexpr mapped_type at(std::string_view key) const
    {
        if (auto it = find(key); it != end())
            return it.value();
        throw out_of_range("no item with given key found");
    }

    // iterator support

    /// Returns an iterator to the first element of the dict_bview.
    /// If the dict_bview is empty, the returned iterator will be equal to end().
    /// @returns Iterator to the first element.
    constexpr const_iterator begin() const noexcept
    {
        if (desc_->is_dict_begin()) {
            // point to first key element of the dict
            return {std::next(desc_), buffer_};
        }
        else {
            return {std::next(desc_, -int(desc_->offset())+1), buffer_};
        }
    }

    /// @copydoc begin()
    constexpr const_iterator cbegin() const noexcept
    { return begin(); }

    /// Returns an iterator to the element following the last element of the dict_bview.
    /// This element acts as a placeholder;
    /// attempting to access it results in undefined behavior.
    /// @returns Iterator to the element following the last element.
    constexpr const_iterator end() const noexcept
    {
        if (desc_->is_dict_begin()) {
            return {std::next(desc_, desc_->offset()), buffer_};
        } else {
            return {desc_, buffer_};
        }
    };


    /// @copydoc end()
    constexpr const_iterator cend() const noexcept
    { return end(); }

    /// Returns a reverse iterator to the first element of the reversed dict_bview.
    /// It corresponds to the last element of the non-reversed dict_bview.
    /// If the dict_bview is empty, the returned iterator is equal to rend().
    constexpr const_reverse_iterator rbegin() const noexcept
    { return const_reverse_iterator{end()}; }

    /// @copydoc rbegin()
    constexpr const_reverse_iterator crbegin() const noexcept
    { return rbegin(); }

    /// Returns a reverse iterator to the element following the last element of the reversed dict_bview.
    /// It corresponds to the element preceding the first element of the non-reversed dict_bview.
    /// This element acts as a placeholder, attempting to access it results in undefined behavior.
    constexpr const_reverse_iterator rend() const noexcept
    { return const_reverse_iterator{begin()}; }

    /// @copydoc rend()
    constexpr const_reverse_iterator crend() const noexcept
    { return rend(); }

    // capacity

    /// Checks if the container has no elements, i.e. whether begin() == end().
    /// @returns true if the container is empty, false otherwise.
    [[nodiscard]]
    constexpr bool empty() const noexcept
    { return desc_->size() == 0; }

    /// Returns the number of elements in the container, i.e. std::distance(begin(), end()).
    /// @returns The number of elements in the container.
    constexpr std::size_t size() const noexcept
    { return desc_->size(); }

    /// Returns the maximum number of elements the container is able to hold.
    /// i.e. std::distance(begin(), end()) for the largest container.
    /// @returns Maximum number of elements.
    constexpr std::size_t max_size() const noexcept
    { return std::numeric_limits<decltype(std::declval<descriptor>().size())>::max(); }

    // lookup

    /// Returns the number of elements with key that compares equivalent to the specified argument,
    /// which is either 1 or 0 since this container does not allow duplicates.
    /// @param key value of the elements to count
    /// @returns Number of elements with key that compares equivalent to key or x, which is either 1 or 0.
    /// @complexity linear in the size of the container.
    constexpr size_type count(std::string_view key) const noexcept
    {
        return std::count_if(begin(), end(),
                [=](auto v) { return (v.first == key); });
    }

    /// Finds an element with key equivalent to key.
    /// @param key value of the element to search for
    /// @returns Iterator to an element with key equivalent to key.
    ///          If no such element is found, past-the-end (see end()) iterator is returned.
    /// @complexity linear in the size of the container.
    constexpr const_iterator find(std::string_view key) const noexcept
    {
        auto it = begin();
        for ( ; it != end(); ++it) {
            if (it->first == key) return it;
        }
        return it;
    }

    /// Checks if there is an element with key equivalent to key in the container.
    /// @param key value of the element to search for
    /// @returns true if there is such an element, otherwise false.
    /// @complexity Linear in the size of the container.
    constexpr bool contains(std::string_view key) const noexcept
    { return count(key) > 0; }

    /// Returns a range containing all elements with the given key in the container.
    /// The range is defined by two iterators, one pointing to the first element
    /// that is not less than key and another pointing to the first element greater than key.
    /// Alternatively, the first iterator may be obtained with lower_bound(), and the second with upper_bound().
    /// @param key value to compare the elements to
    /// @returns std::pair containing a pair of iterators defining the wanted range
    ///         the first pointing to the first element that is not less than key
    ///         and the second pointing to the first element greater than key.
    ///         If there are no elements not less than key, past-the-end (@see end())
    ///         iterator is returned as the first element.
    ///         Similarly if there are no elements greater than key,
    ///         past-the-end iterator is returned as the second element.
    constexpr std::pair<const_iterator, const_iterator>
    equal_range(std::string_view key) const noexcept
    { return {lower_bound(key), upper_bound(key)}; }

    /// Returns an iterator pointing to the first element that is not less than (i.e. greater or equal to) key.
    /// @param key value to compare the elements to
    /// @returns Iterator pointing to the first element that is not less than key.
    ///         If no such element is found, a past-the-end iterator (@see end()) is returned.
    constexpr const_iterator lower_bound(std::string_view key) const noexcept
    { return find(key); }

    /// Returns an iterator pointing to the first element that is greater than key.
    /// @param key value to compare the elements to
    /// @returns Iterator pointing to the first element that is greater than key.
    ///         If no such element is found, past-the-end (@see end()) iterator is returned.
    constexpr const_iterator upper_bound(std::string_view key) const noexcept
    {
        auto it = find(key);
        return (it != end()) ? std::next(it) : it;
    }

    /// Compare equality with a dict_bview.
    ///@param rhs a value to compare
    constexpr bool operator==(const dict_bview& rhs) const noexcept {
        return std::equal(begin(), end(), rhs.begin(), rhs.end());
    }

    /// Compare order with a dict_bview.
    ///@param rhs a value to compare
    constexpr std::weak_ordering operator<=>(const dict_bview& rhs) const noexcept
    {
        return std::lexicographical_compare_three_way(begin(), end(), rhs.begin(), rhs.end());
    }

    using bview::operator==;
    using bview::operator<=>;
};
static_assert(std::three_way_comparable<dict_bview::value_type> , "internal error");

} // namespace bencode

BENCODE_SERIALIZES_TO_DICT_SORTED(bencode::dict_bview);