#pragma once

#include <iterator>
#include <concepts>
#include <ranges>

#include <gsl-lite/gsl-lite.hpp>

#include "bencode/detail/bview/concepts.hpp"
#include "bencode/detail/symbol.hpp"
#include "bencode/detail/out_of_range.hpp"

namespace bencode {

namespace rng = std::ranges;

namespace detail {
class list_bview_iterator
{
public:
    using iterator_type = list_bview_iterator;
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = bview;
    using reference = bview&;
    using const_reference = const bview&;
    using pointer = value_type*;
    using const_pointer = const value_type*;

    using difference_type = std::ptrdiff_t;

    constexpr list_bview_iterator() noexcept
            : data_(nullptr), buffer_(nullptr), begin_(nullptr) {};

    constexpr list_bview_iterator(const list_bview_iterator&) noexcept = default;

    constexpr list_bview_iterator(const descriptor* data, const char* buffer) noexcept
            : data_(data)
            , buffer_(buffer)
            , begin_(data)
    {
        Expects(data_ != nullptr);
        Expects(buffer_ != nullptr);
    }

    constexpr iterator_type& operator=(const list_bview_iterator&) noexcept = default;

    constexpr const_reference operator* () const noexcept
    {
        Expects(!is_end_iterator());
        dereference_buffer_ = {data_, buffer_};
        return dereference_buffer_;
    }

    constexpr const_pointer operator->() const noexcept
    {
        Expects(!is_end_iterator());
        dereference_buffer_ = {data_, buffer_};
        return &dereference_buffer_;
    }

    constexpr iterator_type& operator++() noexcept
    {
        Expects(!is_end_iterator());
        // skip over the tokens of a structured bvalue
        if (data_->is_list_begin() || data_->is_dict_begin()) {
            std::advance(data_, data_->offset()+1);
        }
        else {
            std::advance(data_, 1);
        }
        return *this;
    }

    constexpr iterator_type operator++(int) noexcept
    {
        auto copy = *this;
        operator++();
        return copy;
    }

    constexpr iterator_type& operator--() noexcept
    {
        std::advance(data_, -1);
        if (data_->is_list_end() || data_->is_dict_end()) {
            std::advance(data_, -int(data_->offset()));
        }
        return *this;
    }

    constexpr auto operator--(int) noexcept -> iterator_type
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
    { return data_->is_list_end() && (data_ - data_->offset())->is_list_begin(); }

    const descriptor* data_;
    const char* buffer_;
    mutable value_type dereference_buffer_ {};
    const descriptor* begin_;
};
}

static_assert(std::bidirectional_iterator<detail::list_bview_iterator>, "internal error");

/// The class list_bview is a bview of which the descriptor describes a list value.
/// list_bview does not holds any additional members over bview.
/// This class provides a similar interface to a read-only list.
class list_bview : public bview
{
public:
    using value_type = bview;
    using pointer = bview*;
    using const_pointer = const bview*;
    using reference = bview&;
    using const_reference = const bview&;
    using const_iterator = detail::list_bview_iterator;
    using iterator = const_iterator;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    using reverse_iterator = const_reverse_iterator;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    /// Default constructor. Constructs an empty list_bview.
    /// After construction, data() is equal to nullptr, and size() is equal to 0.
    constexpr explicit list_bview()
            : bview()
    {}

    /// Copy constructor. Constructs an list_bview from a list_bview.
    /// Behavior is undefined when the bview type is not a list_bview.
    constexpr explicit list_bview(const bview& bref) noexcept
            : bview(bref)
    {
        Expects(desc_->is_list());
    }

    /// Constructs a list_bview from a descriptor and a character buffer.
    /// Behavior is undefined when the descriptor type is not a list.
    constexpr list_bview(const descriptor* data, const char* buffer) noexcept
            :bview(data, buffer)
    {
        Expects(desc_->is_list());
    }

    /// Copy constructor.
    constexpr list_bview(const list_bview&) noexcept = default;

    /// Copy assignment.
    constexpr list_bview& operator=(const list_bview&) noexcept = default;

    // iterator support

    /// Returns an iterator to the first element of the list_bview.
    /// @returns Iterator to the first element
    constexpr const_iterator begin() const noexcept
    {
        if (desc_->is_list_begin()) {
            return {std::next(desc_), buffer_};
        }
        else {
            return {std::next(desc_, -int(desc_->offset())+1), buffer_};
        }
    }

    /// @copydoc begin()
    constexpr const_iterator cbegin() const noexcept
    { return begin(); }

    /// Returns an iterator to the element following the last element.
    /// This element acts as a placeholder; attempting to access it results in undefined behavior.
    /// @returns Iterator to the character following the last element.
    constexpr const_iterator end() const noexcept
    {
        if (desc_->is_list_begin()) {
            return {std::next(desc_, desc_->offset()), buffer_};
        } else {
            return {desc_, buffer_};
        }
    };

    /// @copydoc end()
    constexpr const_iterator cend() const noexcept
    { return end(); }

    /// Returns a reverse iterator to the first element of the reversed vector.
    /// It corresponds to the last element of the non-reversed vector.
    /// If the vector is empty, the returned iterator is equal to rend().
    /// @returns Reverse iterator to the first element.
    constexpr const_reverse_iterator rbegin() const noexcept
    { return const_reverse_iterator{end()}; }

    /// @copydoc rbegin()
    constexpr const_reverse_iterator crbegin() const noexcept
    { return rbegin(); }

    /// Returns a reverse iterator to the element following the last element of the reversed vector.
    /// It corresponds to the element preceding the first element of the non-reversed vector.
    /// This element acts as a placeholder, attempting to access it results in undefined behavior.
    /// @returns Reverse iterator to the element following the last element.
    constexpr const_reverse_iterator rend() const noexcept
    { return const_reverse_iterator{begin()}; }

    /// @copydoc rend()
    constexpr const_reverse_iterator crend() const noexcept
    { return const_reverse_iterator{begin()}; }

//-------------------------------------------------------------------------------------------------
// Capacity
//-------------------------------------------------------------------------------------------------

    /// Checks if the container has no elements, i.e. whether begin() == end().
    /// @returns true if the container is empty, false otherwise.
    [[nodiscard]]
    constexpr bool empty() const noexcept
    { return desc_->size() == 0; }

    /// Returns the number of elements in the container, i.e. std::distance(begin(), end()).
    /// @returns The number of elements in the container.
    constexpr size_type size() const noexcept
    { return desc_->size(); }

    /// Returns the maximum number of elements the container is able to hold,
    /// i.e. std::distance(begin(), end()) for the largest container.
    /// @returns Maximum number of elements.
    constexpr size_type max_size() const noexcept
    { return std::numeric_limits<decltype(std::declval<descriptor>().size())>::max(); }

//-------------------------------------------------------------------------------------------------
// element access
//-------------------------------------------------------------------------------------------------

    /// Returns a reference to the element at specified location pos.
    /// No bounds checking is performed.
    /// Accessing element out of bound is undefined behavior.
    /// @param pos position of the element to return
    /// @returns Reference to the requested element.
    /// @complexity linear in size of the container
    constexpr value_type operator[](std::size_t pos) const noexcept
    {
        Expects(pos < size());
        return *std::next(begin(), pos);
    }

    /// Returns a reference to the element at specified location pos.
    /// With bounds checking.
    /// If pos is not within the range of the container, an exception of type std::out_of_range is thrown.
    /// @param pos position of the element to return
    /// @returns Reference to the requested element.
    /// @complexity linear in size of the container
    constexpr value_type at(std::size_t pos) const
    {
        if (pos >= size())
            throw out_of_range("element index out of range");
        return *std::next(begin(), pos);
    };

    /// Returns a reference to the first element in the container.
    /// Calling front on an empty container is undefined.
    /// @returns Reference to the first element.
    constexpr value_type front() const noexcept
    {
        if (desc_->is_list_begin()) {
            return {std::next(desc_), buffer_};
        }
        else {
            return {std::next(desc_, -int(desc_->offset())+1), buffer_};
        }
    }

    /// Returns reference to the last element in the container.
    /// Calling back on an empty container causes undefined behavior.
    /// Reference to the last element.
    constexpr value_type back() const noexcept
    {
        if (desc_->is_list_begin()) {
            return {std::next(desc_, int(desc_->offset())-1), buffer_};
        }
        else {
            return {std::next(desc_, -1), buffer_};
        }
    }

    /// Compare equality with a list_bview.
    ///@param rhs a bvalue to compare
    constexpr bool operator==(const list_bview& rhs) const noexcept
    {
        return std::equal(begin(), end(), rhs.begin(), rhs.end());
    }

    /// Compare equality with a list_bview.
    ///@param rhs a bvalue to compare
    constexpr std::weak_ordering operator<=>(const list_bview& rhs) const noexcept
    {
        return std::lexicographical_compare_three_way(begin(), end(), rhs.begin(), rhs.end());
    }

    using bview::operator==;
    using bview::operator<=>;
};

static_assert(std::bidirectional_iterator<list_bview::iterator>);
static_assert(rng::sized_range<list_bview>);

} // namespace bencode

BENCODE_SERIALIZES_TO_LIST(bencode::list_bview);

