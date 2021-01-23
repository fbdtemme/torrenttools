#pragma once

#include <cstdint>
#include <compare>

namespace cliprogress {

enum class size_policy_flag : std::uint8_t
{
    fixed = 0,     ///< widget cannot resize
    grow = 1,      ///< widget can grow to maximum size
    shrink = 2,    ///< widget can shrink to minimum size
    expand = 4,    ///< widget wants as much space as possible.
};

constexpr size_policy_flag operator~(size_policy_flag lhs) noexcept
{
    return size_policy_flag(~static_cast<std::uint8_t>(lhs));
}

constexpr size_policy_flag operator|(size_policy_flag lhs, size_policy_flag rhs) noexcept
{
    return size_policy_flag(static_cast<std::uint8_t>(lhs) | static_cast<std::uint8_t>(rhs));
};

constexpr size_policy_flag& operator|=(size_policy_flag& lhs, size_policy_flag rhs) noexcept
{
    lhs = size_policy_flag(static_cast<std::uint8_t>(lhs) | static_cast<std::uint8_t>(rhs));
    return lhs;
}

constexpr size_policy_flag operator&(size_policy_flag lhs, size_policy_flag rhs) noexcept
{
    return size_policy_flag(static_cast<std::uint8_t>(lhs) & static_cast<std::uint8_t>(rhs));
};

constexpr size_policy_flag& operator&=(size_policy_flag& lhs, size_policy_flag rhs) noexcept
{
    lhs = size_policy_flag(static_cast<std::uint8_t>(lhs) & static_cast<std::uint8_t>(rhs));
    return lhs;
}

//-----------------------------------------------------------------------------------------------//

/// Expresses a widgets willingness to be resized by a layout.
class size_policy
{
public:
    size_policy() = default;

    size_policy(size_policy_flag flags, std::size_t growth_factor = 1)
            : value_(flags)
            , resize_factor_(growth_factor)
    {}

    /// A widget can grow to the maximum size
    bool is_fixed() const noexcept
    {
        return (static_cast<std::underlying_type_t<size_policy_flag>>(value_) == 0);
    }

    bool can_grow() const noexcept
    {
        return (value_ & size_policy_flag::grow) == size_policy_flag::grow;
    }

    /// A widget can shrink to the minimum size
    bool can_shrink() const noexcept
    {
        return (value_ & size_policy_flag::shrink) == size_policy_flag::shrink;
    }

    /// A widget prefers to take all available space.
    bool is_expanding() const noexcept
    {
        return (value_ & size_policy_flag::expand) == size_policy_flag::expand;
    }

    /// Grow relative to other widgets that can grow.
    void set_resize_factor(std::size_t factor) noexcept
    {
        resize_factor_ = factor;
    }

    std::size_t resize_factor() const noexcept
    {
        return resize_factor_;
    }

    bool operator==(const size_policy& that) const noexcept = default;

private:
    size_policy_flag value_;
    std::size_t resize_factor_;
};

} // namespace cliprogress
