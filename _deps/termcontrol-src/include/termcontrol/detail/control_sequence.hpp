#pragma once
#include <array>
#include <cstdint>
#include <string_view>
#include <limits>
#include <charconv>
#include <cassert>
#include <concepts>
#include <ranges>
#include <algorithm>
#include <compare>
#include <gsl-lite/gsl-lite.hpp>

#include "control_sequence_definition.hpp"
#include "format.hpp"

namespace termcontrol {

namespace rng = std::ranges;

/// Immutable string for holding control sequences.
/// The size template parameter is deduced from the given control_sequence_definition `Def`
/// and parameters are checked at compile-time against the control_sequence_definition.
template <control_sequence_definition Def, std::size_t N = detail::max_formatted_size_v<Def>>
struct control_sequence
{
    using definition = Def;
    using traits_type = std::char_traits<char>;
    using value_type = char;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using const_iterator = const value_type*;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    using const_reference = const value_type&;
    using const_pointer = const value_type*;

    using string_view_type = std::basic_string_view<value_type>;

    static constexpr std::size_t capacity = N;

    template <typename... Ts>
        requires call_signature_matches<definition, Ts...> &&
                (!detail::first_element_is_control_sequence_definition_tag<Ts...>)
        constexpr explicit control_sequence(Ts... parameters) noexcept
            : buffer_()
            , size_()
    {
        auto out = format_to<definition>(buffer_.begin(), std::forward<Ts>(parameters)...);
        size_ = std::distance(buffer_.begin(), out);
    }

    template <typename... Ts>
        requires call_signature_matches<definition, Ts...>
    constexpr explicit control_sequence(detail::control_sequence_definition_tag<definition>,
                                              Ts... parameters) noexcept
            : buffer_()
            , size_()
    {
        auto out = format_to<definition>(buffer_.begin(), std::forward<Ts>(parameters)...);
        size_ = std::distance(buffer_.begin(), out);
    }

    constexpr control_sequence(const control_sequence& other) noexcept = default;
    constexpr control_sequence& operator=(const control_sequence& other) noexcept = default;

    static constexpr auto max_size() noexcept -> size_type
    { return capacity; }

    constexpr auto size() const noexcept -> size_type
    { return size_; }

    constexpr auto data() const noexcept -> const_pointer
    { return buffer_.data(); }

    constexpr auto begin() const noexcept -> const_iterator
    { return buffer_.begin(); }

    constexpr auto end() const noexcept -> const_iterator
    { return std::next(buffer_.data(), size_); }

    constexpr auto cbegin() const noexcept -> const_iterator
    { return begin(); };

    constexpr auto cend() const noexcept -> const_iterator
    { return end(); }

    constexpr auto rbegin() const noexcept -> const_reverse_iterator
    { return end(); }

    constexpr auto rend() const noexcept -> const_reverse_iterator
    { return begin(); }

    constexpr auto crbegin() const noexcept -> const_reverse_iterator
    { return rbegin(); }

    constexpr auto crend() const noexcept -> const_reverse_iterator
    { return rend(); }

    constexpr operator string_view_type() const noexcept
    { return {buffer_.data(), size_}; }

    constexpr auto c_str() const noexcept -> const char*
    {
        buffer_[size_] == '\0';
        return buffer_.data();
    }

    auto str() const -> std::string
    { return  {buffer_.data(), size_}; }

    template <std::size_t N1, typename CSDef>
    constexpr bool operator==(const control_sequence<CSDef, N>& that) const
    { return string_view_type(*this) == string_view_type(*that); }

    constexpr bool operator==(string_view_type that) const
    { return string_view_type(*this) == that; }

    template <std::size_t N1, typename CSDef>
    constexpr std::weak_ordering operator<=>(const control_sequence<CSDef, N>& that) const
    { return std::compare_weak_order_fallback(string_view_type(*this), string_view_type(that)); }

    constexpr std::weak_ordering operator<=>(string_view_type that) const
    { return std::compare_weak_order_fallback(string_view_type(*this), that); }

    // maximum required space:
    //      max 2 char : control_sequence
    //          1 char : param introducer (private sequences)
    //          1 char : null byte
    //
    //          5 chars per argument (max 5 digits: enough for all 16 bit values (max recognised by xterm)
    //          + 1 char per arg-1
    //          1 char : null byte
private:
    mutable std::array<char, capacity> buffer_;
    size_type size_;
};


template <typename Def, typename... Ts>
control_sequence(detail::control_sequence_definition_tag<Def>, Ts... parameters) -> control_sequence<Def>;


template <typename CharT, typename CSDef, std::size_t N, typename Traits=std::char_traits<CharT>>
auto operator<<(std::basic_ostream<CharT, Traits>& os, control_sequence<CSDef, N> s)
  -> std::basic_ostream<CharT, Traits>&
{
    os.write(s.data(), s.size());
    return os;
}

} // namespace termcontrol;
