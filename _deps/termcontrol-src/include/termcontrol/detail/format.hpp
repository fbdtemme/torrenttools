#pragma once
#include <array>
#include <type_traits>
#include <utility>
#include <limits>
#include <bitset>
#include <string>
#include <string_view>
#include <iterator>
#include <concepts>
#include <ranges>


#include <fmt/format.h>
#include <fmt/core.h>

#include "formatted_size.hpp"
#include "concepts.hpp"
#include "parameters.hpp"
#include "color.hpp"
#include "control_sequence_definition.hpp"
#include "definitions.hpp"
#include "text_style.hpp"

namespace termcontrol::detail {

template <typename UnsignedInteger>
constexpr unsigned base_two_digits(UnsignedInteger x)
{
    static_assert(std::is_unsigned_v<UnsignedInteger>, "integer must be unsigned");

    if constexpr (sizeof(x) > 4)
        return x ? 8 * sizeof(x) - __builtin_clzll(x) : 0;
    else
        return x ? 8 * sizeof(x) - __builtin_clz(x) : 0;
}

template <typename UnsignedInteger>
constexpr unsigned base_ten_digits(UnsignedInteger x)
{
    static_assert(std::is_unsigned_v<UnsignedInteger>, "integer must be unsigned");

    if (x == 0)
        return 0;

    constexpr std::array<std::uint64_t, 20> ten_to_the{
            1u,
            10u,
            100u,
            1000u,
            10000u,
            100000u,
            1000000u,
            10000000u,
            100000000u,
            1000000000u,
            10000000000u,
            100000000000u,
            1000000000000u,
            10000000000000u,
            100000000000000u,
            1000000000000000u,
            10000000000000000u,
            100000000000000000u,
            1000000000000000000u,
            10000000000000000000u,
    };

    constexpr std::array<unsigned, 65> guess{
            0, 0, 0, 0, 1, 1, 1, 2, 2, 2,
            3, 3, 3, 3, 4, 4, 4, 5, 5, 5,
            6, 6, 6, 6, 7, 7, 7, 8, 8, 8,
            9, 9, 9, 9, 10, 10, 10, 11, 11, 11,
            12, 12, 12, 12, 13, 13, 13, 14, 14, 14,
            15, 15, 15, 15, 16, 16, 16, 17, 17, 17,
            18, 18, 18, 18, 19
    };

    const auto digits = guess[base_two_digits(x)];
    return digits+(x >= ten_to_the[digits]);
}

using char_pairs = std::pair<char, char>;

constexpr auto generate_lookup_table() noexcept
{
    std::array<char_pairs, 100> table{};
    std::size_t idx = 0;

    for (char i = '0'; i <= '9'; ++i) {
        for (char j = '0'; j <= '9'; ++j, ++idx) {
            table[idx].first = i;
            table[idx].second = j;
        }
    }
    return table;
}

inline static constexpr auto digits = generate_lookup_table();

/// Fast integer formatter.
/// Adapted from fmt::format_int.

template <std::size_t N, typename CharT = char>
class format_int {
    // 1 extra char for terminating null-byte
    static constexpr std::size_t buffer_size = N * sizeof(CharT)+1;
public:
    constexpr explicit format_int(int value)
            : buffer_(), str_(buffer_.data()) { format_signed(value); }

    constexpr explicit format_int(long value)
            : buffer_(), str_(buffer_.data()) { format_signed(value); }

    constexpr explicit format_int(long long value)
            : buffer_(), str_(buffer_.data()) { format_signed(value); }

    constexpr explicit format_int(std::uint8_t value)
            : buffer_(), str_(format_decimal(value)) { }

    constexpr explicit format_int(std::uint16_t value)
            : buffer_(), str_(format_decimal(value)) { }

    constexpr explicit format_int(std::uint32_t value)
            : buffer_(), str_(format_decimal(value)) { }

    constexpr explicit format_int(std::uint64_t value)
            : buffer_(), str_(format_decimal(value)) { }

    /// Returns the number of characters written to the output buffer.
    constexpr std::size_t size() const noexcept
    { return std::distance(const_cast<const CharT*>(str_), buffer_.end()-1); }

    /// Returns a pointer to the output buffer content.
    constexpr const CharT* data() const noexcept
    { return str_; }

    constexpr const CharT* begin() const noexcept
    { return str_; }

    constexpr const CharT* end() const noexcept
    { return buffer_.end()-1; }

    explicit constexpr operator std::string_view() const noexcept
    { return std::string_view(str_, size()); };

    /// Returns a pointer to the output buffer content with terminating null
    /// character appended.
    [[nodiscard]]
    constexpr auto c_str() const noexcept -> const CharT*
    {
        buffer_[buffer_size-1] = CharT('\0');
        return str_;
    }

    /// Returns the content of the output buffer as an ``std::string``.
    [[nodiscard]]
    std::string str() const { return std::string(str_, size()); }

private:
    // Formats value in reverse and returns a pointer to the beginning.
    constexpr CharT* format_decimal(unsigned long long value)
    {
        CharT* ptr = buffer_.data()+(buffer_.size()-1);

        while (value >= 100) {
            // Integer division is slow so do it for a group of two digits instead
            // of for every digit. The idea comes from the talk by Alexandrescu
            // "Three Optimization Tips for C++". See speed-test for a comparison.
            auto index = static_cast<unsigned>((value % 100));
            value /= 100;
            *--ptr = CharT(digits[index].second);
            *--ptr = CharT(digits[index].first);
        }
        if (value < 10) {
            *--ptr = static_cast<CharT>('0'+value);
            return ptr;
        }
        auto index = static_cast<unsigned>(value);
        *--ptr = CharT(digits[index].second);
        *--ptr = CharT(digits[index].first);
        return ptr;
    }

    constexpr void format_signed(long long value)
    {
        auto abs_value = static_cast<unsigned long long>(value);
        bool negative = value < 0;
        if (negative) { abs_value = 0-abs_value; }
        str_ = format_decimal(abs_value);
        if (negative) { *--str_ = CharT('-'); }
    }

    // Buffer should be large enough to hold all digits (digits10 + 1),
    // a sign and a null character.
    std::array<CharT, buffer_size> buffer_;
    CharT* str_;
};


/// CTAD deduction guides

template <typename CharT = char>
format_int(std::uint8_t n) -> format_int<max_formatted_size_v < std::uint8_t>, CharT>;

template <typename CharT = char>
format_int(std::uint16_t n) -> format_int<max_formatted_size_v < std::uint16_t>, CharT>;

template <typename CharT = char>
format_int(std::uint32_t n) -> format_int<max_formatted_size_v < std::uint32_t>, CharT>;

template <typename CharT = char>
format_int(std::uint64_t n) -> format_int<max_formatted_size_v < std::uint64_t>, CharT>;

template <typename CharT = char>
format_int(std::int8_t n) -> format_int<max_formatted_size_v < std::int8_t>, CharT>;

template <typename CharT = char>
format_int(std::int16_t n) -> format_int<max_formatted_size_v < std::int16_t>, CharT>;

template <typename CharT = char>
format_int(std::int32_t n) -> format_int<max_formatted_size_v < std::int32_t>, CharT>;

template <typename CharT = char>
format_int(std::int64_t n) -> format_int<max_formatted_size_v < std::int64_t>, CharT>;


template <typename T>
concept control_sequence_parameter = requires(T) {
    detail::max_formatted_size<T>::value;
};

//============================================================================//
// Parameter formatters                                                       //
//============================================================================//


template <typename OutputIterator, control_sequence_parameter T>
    requires std::integral<T>
constexpr auto format_parameter(OutputIterator out, T x) -> OutputIterator
{
    const auto v = format_int(x);
    out = std::copy(v.begin(), v.end(), out);
    return out;
}

/// Format enums to their values but with different buffer size
template <typename OutputIterator, control_sequence_parameter EnumT>
    requires std::is_enum_v<EnumT>
constexpr auto format_parameter(OutputIterator out, EnumT value) -> OutputIterator
{
    const auto v = detail::format_int<max_formatted_size_v<EnumT>>(
            std::underlying_type_t<EnumT>(value));
    out = std::copy(v.begin(), v.end(), out);
    return out;
}

template <typename OutputIterator>
constexpr auto format_parameter(OutputIterator out, dec_mode value) -> OutputIterator
{
    *out++ = '?';
    return format_parameter(out, std::underlying_type_t<dec_mode>(value));
}

template <typename OutputIterator>
constexpr auto format_parameter(OutputIterator out, graphics_rendition_attribute value) -> OutputIterator
{
    return format_parameter(out, std::underlying_type_t<graphics_rendition_attribute>(value));
}

/// Format enums to their values but with different buffer size
template <typename OutputIterator>
constexpr auto format_parameter(OutputIterator out, rgb_color value) -> OutputIterator
{
    out = format_parameter(out, value.r); *out++ = ';';
    out = format_parameter(out, value.g); *out++ = ';';
    out = format_parameter(out, value.b);
    return out;
}

template <typename OutputIterator>
constexpr auto format_parameter(OutputIterator out, emphasis e) -> OutputIterator
{
    constexpr std::array flags = {
            emphasis::reset,
            emphasis::bold,
            emphasis::italic,
            emphasis::underline,
            emphasis::striketrough,
            emphasis::overline,
            emphasis::double_underline,
            emphasis::blinking,
            emphasis::reverse,
    };
    constexpr std::array values {"0", "1", "3", "4", "9", "53", "21", "5", "7"};

    bool needs_seperator = false;
    for (std::size_t i = 0; i < flags.size(); ++i) {
        if ((e & flags[i]) == flags[i]) {
            if (needs_seperator) *out++ = ';';
            for (const char c: std::string_view(values[i])) *out++ = c;
            needs_seperator = true;
        }
    }
    return out;
}

template <typename OutputIterator>
constexpr auto format_parameter(OutputIterator out, area_qualification m) -> OutputIterator
{
    constexpr std::array flags = {
            area_qualification::unprotected_unguarded,
            area_qualification::protected_guarded,
            area_qualification::graphic,
            area_qualification::numeric,
            area_qualification::alphabetic,
            area_qualification::aligned_end,
            area_qualification::filled_zero,
            area_qualification::set_character_tab,
            area_qualification::protected_unguarded,
            area_qualification::filled_space,
            area_qualification::align_start,
            area_qualification::reverse_character_input,
    };

    bool needs_seperator = false;
    for (std::size_t i = 0; i < flags.size(); ++i) {
        if ((m & flags[i]) == flags[i]) {
            if (needs_seperator) *out++ = ';';
            out = format_parameter(out, i);
            needs_seperator = true;
        }
    }
    return out;
}

template <typename OutputIterator>
constexpr auto format_parameter(OutputIterator out, cursor_tabulation_control_mode m) -> OutputIterator
{
    using tcm = cursor_tabulation_control_mode;
    using underlying_t = std::underlying_type_t<cursor_tabulation_control_mode>;
    constexpr std::array flags = {
            tcm::set_character_tab,
            tcm::set_line_tab,
            tcm::clear_character_tab,
            tcm::clear_line_tab,
            tcm::clear_character_tab_line,
            tcm::clear_character_tab_all,
            tcm::clear_line_tab_all,
    };
    constexpr std::array valid {
            tcm::set_character_tab        | tcm::set_line_tab,
            tcm::set_character_tab        | tcm::clear_line_tab,
            tcm::set_character_tab        | tcm::clear_line_tab_all,
            tcm::set_line_tab             | tcm::clear_character_tab,
            tcm::set_line_tab             | tcm::clear_character_tab_line,
            tcm::set_line_tab             | tcm::clear_character_tab_all,
            tcm::clear_character_tab      | tcm::clear_line_tab,
            tcm::clear_character_tab      | tcm::clear_line_tab_all,
            tcm::clear_line_tab           | tcm::clear_character_tab_line,
            tcm::clear_line_tab           | tcm::clear_character_tab_all,
            tcm::clear_character_tab_line | tcm::clear_line_tab_all,
            tcm::clear_character_tab_all  | tcm::clear_line_tab_all
    };
    auto bit_count = std::popcount(static_cast<underlying_t>(m));

    if (bit_count > 2) {
        throw std::invalid_argument("tab_control_mode has more than two flags set");
    }
    else if (bit_count == 2) {
        bool in_valid = false;
        for (auto v : valid) {
            if (m == v) {
                in_valid = true;
                break;
            }
        }
        if (!in_valid) {
            throw std::invalid_argument("tab_control_mode has an invalid combination of flags set");
        }
    }

    bool needs_seperator = false;
    for (std::size_t i = 0; i < flags.size(); ++i) {
        if ((m & flags[i]) == flags[i]) {
            if (needs_seperator) *out++ = ';';
            out = format_parameter(out, i);
            needs_seperator = true;
        }
    }
    return out;
}

template <typename OutputIterator>
constexpr auto format_parameter(OutputIterator out, const text_style& s) -> OutputIterator
{
    bool needs_seperator = false;

    if (s.has_emphasis()) {
        out = format_parameter(out, s.get_emphasis());
        needs_seperator = true;
    }
    if (s.has_foreground_color()) {
        if (needs_seperator) *out++ = ';';
        auto fg_color = s.get_foreground_color();
        if (fg_color.is_rgb_color()) {
            for (const char c: "38;2;") *out++ = c;
            out = format_parameter(out, fg_color.get_rgb_color());
        }
        else {
            out = format_parameter(out, std::uint8_t(fg_color.get_terminal_color()));
        }
        needs_seperator = true;
    }
    if (s.has_background_color()) {
        if (needs_seperator) *out++ = ';';
        auto bg_color = s.get_background_color();
        if (bg_color.is_rgb_color()) {
            for (const char c: "48;2;") *out++ = c;
            out = format_parameter(out, bg_color.get_rgb_color());
        }
        else {
            out = format_parameter(out, std::uint8_t(bg_color.get_terminal_color())+10);
        }
    }
    return out;
}

template <typename OutputIterator, std::size_t... Is, typename... Ts>
constexpr auto format_parameter_string_helper(OutputIterator it, std::index_sequence<Is...>, Ts... args) ->
OutputIterator
{
    std::tuple t = {args...};
    constexpr std::size_t N = sizeof...(Ts);

    ((it = format_parameter(it, std::get<Is>(t)), *it++ = ';'), ...);
    it = format_parameter(it, std::get<N-1>(t));
    return it;
}

template <typename OutputIterator, control_sequence_parameter... Ts>
constexpr auto format_parameter_string(OutputIterator out, Ts... args) -> OutputIterator
{
    constexpr std::size_t N = sizeof...(Ts);

    if constexpr (N == 0)
        return out;
    else
        return format_parameter_string_helper(out, std::make_index_sequence<N-1>{}, args...);
}

} // namespace termcontrol::detail


namespace termcontrol {

namespace rng = std::ranges;

/// Format the specified control sequence with given arguments to a std::string.
/// Write a control sequence formatted with `parameters` according to the control sequence definition `Def` and
/// return the result as a std::string.
template <control_sequence_definition Def, typename... Ts>
    requires call_signature_matches<Def, Ts...> &&
             (!detail::first_element_is_control_sequence_definition_tag<Ts...>)
auto format(Ts... parameters) -> std::string
{
    std::string result {};
    constexpr auto size_guess = detail::max_formatted_size_v<Def>
                                + detail::max_formatted_size_v<Ts...> / 2;
    result.reserve(size_guess);
    auto it = std::back_inserter(result);
    rng::copy(control_functions::control_sequence_introducer, it);

    if constexpr (detail::has_parameter_byte<Def>) {
        *it++ = Def::parameter_introducer;
    }
    // retrieve default parameters if no parameters given;
    if constexpr (sizeof...(Ts) != std::tuple_size_v<typename Def::parameters>
                  && sizeof...(Ts) == 0)
    {
        it = std::apply([&](auto... v) {
            return termcontrol::detail::format_parameter_string(it, v...);
        }, detail::default_parameters<Def>::value);
    }
    else {
        it = termcontrol::detail::format_parameter_string(it, parameters...);
    }
    if constexpr (detail::has_intermediate_bytes<Def>) {
        rng::copy(Def::intermediate_bytes, it);
    }
    *it++ = Def::final_byte;
    return result;
}

/// Format the specified control sequence with given arguments to a std::string.
/// Write a control sequence formatted with `parameters` according to the control sequence definition `Def` and
/// return the result as a std::string.
template <control_sequence_definition Def, typename... Ts>
    requires call_signature_matches<Def, Ts...>
std::string format(detail::control_sequence_definition_tag<Def>, Ts... parameters)
{
    return format<Def>(parameters...);
}

/// writes out the specified control sequence with its arguments through an output iterator.
/// Write a control sequence formatted with `parameters` according to the control sequence definition `Def` and
/// write the result to output iterator `out`.
template <control_sequence_definition Def, typename OIter, typename... Ts>
    requires call_signature_matches<Def, Ts...> &&
            (!detail::first_element_is_control_sequence_definition_tag<Ts...>) &&
            std::output_iterator<OIter, char>
constexpr auto format_to(OIter it, Ts... parameters) -> OIter
{
    auto [in, out] = rng::copy(control_functions::control_sequence_introducer, it);
    it = out;

    if constexpr (detail::has_parameter_byte<Def>) {
        *it++ = Def::parameter_introducer;
    }
    // retrieve default parameters if no parameters given;
    if constexpr (sizeof...(Ts) != std::tuple_size_v<typename Def::parameters>
                  && sizeof...(Ts) == 0)
    {
        it = std::apply([it](auto... v) {
            return termcontrol::detail::format_parameter_string(it, v...);
        }, detail::default_parameters<Def>::value);
    }
    else {
        it = termcontrol::detail::format_parameter_string(it, parameters...);
    }
    if constexpr (detail::has_intermediate_bytes<Def>) {
        auto [in, out] = rng::copy(Def::intermediate_bytes, it);
        it = out;
    }
    *it++ = Def::final_byte;
    return it;
}


template <control_sequence_definition Def, typename OIter, typename... Ts>
requires call_signature_matches<Def, Ts...> &&
        std::output_iterator<OIter, char>
constexpr auto format_to(OIter it, detail::control_sequence_definition_tag<Def>, Ts... parameters) -> OIter
{
    return format_to<Def>(it, parameters...);
}

template <control_sequence_definition Def, typename... Ts>
    requires call_signature_matches<Def, Ts...> &&
            (!detail::first_element_is_control_sequence_definition_tag<Def>)
constexpr auto format_to(std::ostream& os, Ts... parameters) -> std::ostream&
{
    format_to<Def>(std::ostreambuf_iterator{os}, parameters...);
    return os;
}

template <control_sequence_definition Def, typename... Ts>
requires call_signature_matches<Def, Ts...>
constexpr auto format_to(std::ostream& os, detail::control_sequence_definition_tag<Def>, Ts... parameters)
    -> std::ostream&
{
    format_to<Def>(std::ostreambuf_iterator{os}, parameters...);
    return os;
}



//template <control_sequence_definition Def, typename OIter, typename... Ts>
//requires call_signature_matches<Def, Ts...> &&
//        std::output_iterator<OIter, char>
//constexpr auto format_to(OIter it, Ts... parameters) -> OIter;
//

//// TODO: Implement format_to_n.
///// Write a control sequence formatted with `parameters` to output range `out` not exceeding n.

//template <typename OutputIt>
//struct format_to_n_result_t
//{
//    OutputIt out;
//    std::size_t size;
//};
//
//template <control_sequence_definition Def, typename OIter, typename... Ts>
//    requires call_signature_matches<Def, Ts...>
//constexpr auto format_to_n(OIter it, typename std::iterator_traits<OIter>::difference_type n, Ts... parameters)
//       -> format_to_n_result_t<OIter>;
//
//template <control_sequence_definition Def, typename OIter, typename... Ts>
//requires call_signature_matches<Def, Ts...>
//constexpr auto format_to_n(OIter it, typename std::iterator_traits<OIter>::difference_type n, Ts... parameters)
//        -> format_to_n_result_t<OIter>
//{
//    format_to_n<Def>(it, std::forward<Ts>(parameters)...);
//}


} // namespace termcontrol