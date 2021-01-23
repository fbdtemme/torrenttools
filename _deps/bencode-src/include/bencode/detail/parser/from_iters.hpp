#pragma once

#pragma once

#include <concepts>
#include <gsl-lite/gsl-lite.hpp>

#include <ranges>

#include "bencode/detail/concepts.hpp"
#include "bencode/detail/symbol.hpp"
#include "bencode/detail/utils.hpp"
#include "bencode/detail/parser/parsing_error.hpp"
#include "bencode/detail/parser/helpers.hpp"
#include "bencode/detail/parser/from_chars.hpp"
/// @file Parsing routines for bencoded data for generic instruction set architecture

namespace bencode::detail {



namespace rng = std::ranges;

template <typename Iterator>
struct from_iters_result
{
    Iterator iter;
    parsing_errc ec;
};


template <std::integral T, std::size_t ImplementationIdx>
constexpr from_iters_result<const char*>
from_iters(const char* first, const char* last, T& value,
           implementation_tag<ImplementationIdx> tag)
{
    from_chars_result res = from_chars(first, last, value, tag);
    return {.iter=res.ptr, .ec=res.ec};
}



template <std::integral T>
constexpr from_iters_result<const char*> from_iters(const char* first, const char* last, T& value)
{
    return from_iters(first, last, value, implementation::serial);
}


template <std::input_iterator Iterator, std::integral T>
    requires std::convertible_to<typename std::iterator_traits<Iterator>::value_type, char>
constexpr from_iters_result<Iterator> from_iters(Iterator first, Iterator last, T& value)
{
    int sign = 1;
    Iterator start = first;
    from_iters_result<Iterator> res { .iter = first, .ec = parsing_errc{}};

    if (first == last)  [[unlikely]] {
        res.ec = parsing_errc::unexpected_eof;
        return res;
    }

    if constexpr (std::signed_integral<T>) {
        if (*first == '-') {
            sign = -1;
            ++first;
        }
        if (first == last)  [[unlikely]] {
            res.ec = parsing_errc::unexpected_eof;
            return res;
        }
    }

    using UT = typename std::make_unsigned<std::remove_cvref_t<T>>::type;
    constexpr auto max_digits = std::numeric_limits<UT>::digits10;
    UT val = 0;

    // do not advance here since we must read the zero again
    const bool leading_zero = (*first == '0');
    int digits = 0;

    unsigned short v;
    bool valid = true;

    for (; digits < max_digits && (first != last) &&
            ((v = static_cast<unsigned>(*first) - '0'), v <= 9); ++digits, ++first)
    {
        raise_and_add(val, 10, v);
    }

    // use integer safe math to check for overflow if we have only one digit precision left
    // use integer safe math to check for overflow if we have only one digit precision left
    if (digits == max_digits && (first != last)) [[unlikely]] {
        while (v = static_cast<unsigned>(*first) - '0', v <= 9) {
            if (!raise_and_add_safe(val, 10, v)) {
                valid = false;
                break;
            }
        }
    }

    res.iter = first;

    if (digits == 0) [[unlikely]] {
        res.ec = parsing_errc::expected_digit;
        return res;
    }

    if (!valid) [[unlikely]] {
        res.ec = parsing_errc::result_out_of_range;
        return res;
    }
    if (leading_zero && val != 0) [[unlikely]] {
        res.ec = parsing_errc::leading_zero;
        return res;
    }

    if constexpr (std::signed_integral<T>) {
        if (val == 0 && sign == -1) [[unlikely]] {
            res.ec = parsing_errc::negative_zero;
            return res;
        }

        T tmp;
        if (__builtin_mul_overflow(val, sign, &tmp))
            res.ec = parsing_errc::result_out_of_range;
        else
            value = tmp;
    }
    else if constexpr (std::numeric_limits<UT>::max() > std::numeric_limits<T>::max())
    {
        if (val > std::numeric_limits<T>::max())
            res.ec = parsing_errc::result_out_of_range;
        else
            value = val;
    }
    else
        value = val;

    return res;
}


template <std::integral T, std::size_t ImplementationIdx>
constexpr from_iters_result<const char*>
binteger_from_iters(const char* first, const char* last, T& value,
        implementation_tag<ImplementationIdx> tag)
{
    from_chars_result res = binteger_from_chars(first, last, value, tag);
    return {.iter=res.ptr, .ec=res.ec};
}

template <std::integral T>
constexpr from_iters_result<const char*>
binteger_from_iters(const char* first, const char* last, T& value)
{
    from_chars_result res = binteger_from_chars(first, last, value, implementation::serial);
    return {.iter=res.ptr, .ec=res.ec};
}



template <std::input_iterator Iterator, std::integral T>
    requires std::convertible_to<typename std::iterator_traits<Iterator>::value_type, char>
constexpr from_iters_result<Iterator>
binteger_from_iters(Iterator first, Iterator last, T& value)
{
    from_iters_result<Iterator> res { .iter = first, .ec = parsing_errc{}};

    if (first == last) [[unlikely]] {
        res.ec = parsing_errc::unexpected_eof;
        return res;
    }

    if (*first != 'i') [[unlikely]] {
        res.ec = parsing_errc::expected_integer_start_token;
        return res;
    }

    // skip 'i'
    ++first;
    T val;
    from_iters_result<Iterator> from_iters_res = from_iters(first, last, val);

    first = from_iters_res.iter;

    // pass possible errors from parse_integer
    if (from_iters_res.ec != parsing_errc{}) [[unlikely]] {
        res = from_iters_res;
        return res;
    }

    // verify the integer is correctly terminated with the "e"
    if (first == last) [[unlikely]] {
        res.ec = parsing_errc::unexpected_eof;
        res.iter = first;
        return res;
    }
    if (*first != symbol::end) [[unlikely]] {
        res.ec = parsing_errc::expected_end;
        res.iter = first;
        return res;
    }

    ++first;
    res.iter = first;
    value = val;
    return res;
}

template <typename StringT = std::string, std::size_t ImplementationIdx>
    requires std::constructible_from<StringT, const char*, const char*>
constexpr from_iters_result<const char*>
bstring_from_iters(const char* first, const char* last, StringT& value,
                   implementation_tag<ImplementationIdx> tag)
{
    std::size_t offset, size;
    auto res = bstring_from_chars(first, last, offset, size, tag);
    if (res.ec != parsing_errc{}) [[unlikely]] {
        return {.iter = res.ptr, .ec = res.ec};
    }
    value = StringT(std::next(first, offset), std::next(first, offset+size));
    return {.iter=res.ptr, .ec=res.ec};
}

template <typename StringT = std::string>
constexpr from_iters_result<const char*>
bstring_from_iters(const char* first, const char* last, StringT& value)
{
    return bstring_from_iters(first, last, value, implementation::serial);
}

template <typename Iterator, typename StringT = std::string>
constexpr from_iters_result<Iterator> bstring_from_iters(Iterator first, Iterator last, StringT& s)
{
    from_iters_result<Iterator> res { .iter = first, .ec = parsing_errc{}};

    if (first == last) [[unlikely]] {
        res.ec = parsing_errc::unexpected_eof;
        res.iter = first;
        return res;
    }
    if (*first == '-') [[unlikely]] {
        res.ec = parsing_errc::negative_string_length;
        res.iter = first;
        return res;
    }

    std::size_t size;
    const auto from_iters_res = from_iters(first, last, size);

    if (from_iters_res.ec != parsing_errc{}) [[unlikely]] {
        res = from_iters_res;
        return res;
    }
    first = from_iters_res.iter;

    if (first == last) [[unlikely]] {
        res.ec = parsing_errc::unexpected_eof;
        res.iter = first;
        return res;
    }
    if (*first != ':') [[unlikely]] {
        res.ec = parsing_errc::expected_colon;
        res.iter = first;
        return res;
    }

    // move past colon
    ++first;

    // copy string
    // reserve space if the output supports it
    if constexpr (has_reserve_member<StringT>) {
        s.reserve(size);
    }
    if constexpr (std::random_access_iterator<Iterator>) {
        auto str_end = std::next(first, size);

        if (str_end > last) [[unlikely]] {
            res.ec = parsing_errc::unexpected_eof;
            res.iter = first;
            return res;
        }

        s = StringT(first, str_end);
        first = str_end;
    }
    else {
        for (size_t idx = 0; idx < size; ++first, ++idx) {
            if (first == last) [[unlikely]] {
                res.ec = parsing_errc::unexpected_eof;
                res.iter = first;
                return res;
            }
            s.push_back(*first);
        }
    }

    res.iter = first;
    return res;
}


}