#pragma once

#include <span>
#include "bencode/detail/descriptor.hpp"
#include "bencode/detail/bview/bview.hpp"
#include "bencode/detail/bview/comparison.hpp"

namespace bencode {

namespace rng = std::ranges;

/// Data structure that hold a contiguous sequence of descriptors and a pointer to the bencoded data they describe.
///
/// A descriptor_table instance is the result parsing bencoded data with a descriptor_parser.
/// The bencoded data can be accessed using a `bview` instance with get_bview().
/// @see descriptor_parser
class descriptor_table {
public:
    descriptor_table() noexcept
            : buffer_(), descriptors_() { }

    descriptor_table(std::span<descriptor> descriptors, const char* buffer)
            : buffer_(buffer)
            , descriptors_(descriptors.begin(), descriptors.end())
    { }

    descriptor_table(std::vector<descriptor>&& descriptors, const char* buffer) noexcept
            : buffer_(buffer)
            , descriptors_(std::move(descriptors))
    { }

    descriptor_table(const descriptor_table&) = default;

    descriptor_table(descriptor_table&&) = default;

    descriptor_table& operator=(const descriptor_table&) = default;

    descriptor_table& operator=(descriptor_table&&) = default;

    /// Returns a bview instance for to the data described by the descriptor at pos.
    bview get_root(std::size_t pos = 0) const noexcept
    {
        Expects(pos<descriptors_.size());
        return bview(descriptors_.data()+pos, buffer_+pos);
    }

    /// Returns a reference to a std::vector storing the descriptors.
    std::vector<descriptor>& descriptors() noexcept
    {
        return descriptors_;
    }

    /// @copydoc descriptors()
    const std::vector<descriptor>& descriptors() const noexcept
    {
        return descriptors_;
    }

private:
    const char* buffer_;
    std::vector<descriptor> descriptors_;
};

} // namespace bencode