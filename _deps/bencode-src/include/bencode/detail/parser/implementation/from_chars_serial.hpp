#pragma once

#include <cstdint>
#include "bencode/detail/parser/parsing_error.hpp"
#include "bencode/detail/parser/helpers.hpp"

namespace bencode::detail {

constexpr from_chars_result from_chars(
        const char* first, const char* last, bool& value, implementation_tag<0>)
{
    std::uint8_t v = (*first-'0');

    if (v<=1) [[likely]] {
        value = v;
        return {++first, parsing_errc{}};
    }
    else if (v<=9) {
        return {++first, parsing_errc::result_out_of_range};
    }
    else {
        return {first, parsing_errc::expected_digit};
    }
}

template<std::integral T>
constexpr from_chars_result from_chars(
        const char* first, const char* last, T& value, implementation_tag<0>)
{
    int sign = 1;
    const char* start = first;
    from_chars_result res{.ptr = first, .ec = parsing_errc{}};

    if (first==last) [[unlikely]] {
        res.ec = parsing_errc::unexpected_eof;
        return res;
    }

    if constexpr (std::signed_integral<T>) {
        if (*first=='-') {
            sign = -1;
            ++first;
        }
        if (first==last)  [[unlikely]] {
            res.ec = parsing_errc::unexpected_eof;
            return res;
        }
    }

    using UT = typename std::make_unsigned<std::remove_cvref_t<T>>::type;
    constexpr auto max_digits = std::numeric_limits<UT>::digits10;
    UT val = 0;

    // do not advance here since we must read the zero again
    const bool leading_zero = (*first=='0');
    int digits = 0;

    unsigned short v;
    bool valid = true;

    for (; digits<max_digits && (first!=last) &&
            ((v = static_cast<unsigned>(*first)-'0'), v<=9); ++digits, ++first) {
        raise_and_add(val, 10, v);
    }

    // use integer safe math to check for overflow if we have only one digit precision left
    if (digits==max_digits && (first!=last)) [[unlikely]] {
        while (v = static_cast<unsigned>(*first)-'0', v<=9) {
            if (!raise_and_add_safe(val, 10, v)) {
                valid = false;
                break;
            }
        }
    }
    res.ptr = first;

    if (digits==0) [[unlikely]] {
        res.ec = parsing_errc::expected_digit;
        return res;
    }

    if (!valid) [[unlikely]] {
        res.ec = parsing_errc::result_out_of_range;
        return res;
    }
    if (leading_zero && val!=0) [[unlikely]] {
        res.ec = parsing_errc::leading_zero;
        return res;
    }

    if constexpr (std::signed_integral<T>) {
        if (val==0 && sign==-1) [[unlikely]] {
            res.ec = parsing_errc::negative_zero;
            return res;
        }

        T tmp;
        if (__builtin_mul_overflow(val, sign, &tmp))
            res.ec = parsing_errc::result_out_of_range;
        else
            value = tmp;
    }
    else if constexpr (std::numeric_limits<UT>::max()>std::numeric_limits<T>::max()) {
        if (val>std::numeric_limits<T>::max())
            res.ec = parsing_errc::result_out_of_range;
        else
            value = val;
    }
    else
        value = val;

    return res;
}

} // namespace bencode::detail