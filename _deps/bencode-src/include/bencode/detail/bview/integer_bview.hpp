#pragma once

#include <compare>

#include <gsl-lite/gsl-lite.hpp>

#include "bencode/detail/serialization_traits.hpp"
#include "bencode/detail/bview/bview.hpp"

namespace bencode {


/// The class integer_bview is a bview of which the descriptor describes an integer value.
/// integer_bview does not holds any additional members over bview.
class integer_bview : public bview
{
public:
    using value_type = std::int64_t;

    constexpr explicit integer_bview() noexcept
            : bview()
    {}

    constexpr explicit integer_bview(const bview& bref) noexcept
            : bview(bref)
    {
        Expects(desc_->is_integer());
    }

    constexpr integer_bview(const descriptor* data, const char* buffer) noexcept
         : bview(data, buffer)
    {
        Expects(desc_->is_integer());
    }

    /// Copy constructor.
    constexpr integer_bview(const integer_bview&) noexcept = default;

    /// Copy assignment.
    constexpr integer_bview& operator=(const integer_bview&) noexcept = default;

    /// Return the value of the integer.
    /// @returns an integral value
    constexpr auto value() const noexcept -> std::int64_t
    { return desc_->value(); }

    /// Implicit conversion operator.
    /// @returns an integral value
    constexpr operator std::int64_t() const noexcept
    { return value(); }

    /// Compare the content of lhs and rhs for equality.
    /// @param that value to compare
    constexpr bool operator==(const integer_bview& that) const noexcept
    { return this->value() == that.value(); }

    /// Compares the contents of lhs and rhs.
    /// @param that value to compare
    constexpr std::strong_ordering operator<=>(const integer_bview& that) const noexcept
    { return this->value() <=> that.value(); }

};

} // namespace bencode

BENCODE_SERIALIZES_TO_INTEGER(bencode::integer_bview);