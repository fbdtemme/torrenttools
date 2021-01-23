#pragma once

#include <immintrin.h>
#include <nonstd/expected.hpp>

#include "bencode/detail/parser/parsing_error.hpp"
#include "bencode/detail/parser/from_chars.hpp"

/// @file Parsing routines for bencoded data for AVX2 instruction set architecture


namespace bencode::detail::sse41
{
using row = std::array<std::uint8_t, 16>;

inline constexpr auto shuffle_mask_lookup = std::array<row, 17> {
        row{0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
        row{0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
        row{1, 0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
        row{2, 1, 0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
        row{3, 2, 1, 0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
        row{4, 3, 2, 1, 0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
        row{5, 4, 3, 2, 1, 0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
        row{6, 5, 4, 3, 2, 1, 0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
        row{7, 6, 5, 4, 3, 2, 1, 0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
        row{8, 7, 6, 5, 4, 3, 2, 1, 0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
        row{9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
        row{10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0xff, 0xff, 0xff, 0xff, 0xff},
        row{11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0xff, 0xff, 0xff, 0xff},
        row{12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0xff, 0xff, 0xff},
        row{13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0xff, 0xff},
        row{14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0xff},
        row{15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0},
};

inline constexpr auto power_of_10_lookup = std::array {
        1UL,                     // 0
        10UL,                    // 1
        100UL,                   // 2
        1000UL,                  // 3
        10000UL,                 // 4
        100000UL,                // 5
        1000000UL,               // 6
        10000000UL,              // 7
        100000000UL,             // 8
        1000000000UL,            // 9
        10000000000UL,           // 10
        100000000000UL,          // 11
        1000000000000UL,         // 12
        10000000000000UL,        // 13
        100000000000000UL,       // 14
        1000000000000000UL,      // 15
        10000000000000000UL,     // 16
        100000000000000000UL,    // 17
        1000000000000000000UL,   // 18
        10000000000000000000UL,  // 19
};


inline std::uint32_t decimal_digits_mask(__m128i input)
{
    // This moves codes of '0'..'9' to range 118, 127.
    // Then it is sufficient to compare the subtracted value for greater then 117.
    __m128i offset = _mm_set1_epi8(70);
    __m128i comp = _mm_set1_epi8(117);
    __m128i t0 = _mm_add_epi8(input, offset);
    __m128i mask = _mm_cmpgt_epi8(t0, comp);
    return _mm_movemask_epi8(mask);
}

inline std::uint64_t decimal_digits_mask(__m128i input_lo, __m128i input_hi)
{
    // This moves codes of '0'..'9' to range 118, 127.
    // Then it is sufficient to compare the subtracted value for greater then 117.
    __m128i offset = _mm_set1_epi8(70);
    __m128i comp = _mm_set1_epi8(117);
    __m128i t0_lo = _mm_add_epi8(input_lo, offset);
    __m128i t0_hi = _mm_add_epi8(input_hi, offset);
    __m128i mask_lo = _mm_cmpgt_epi8(t0_lo, comp);
    __m128i mask_hi = _mm_cmpgt_epi8(t0_hi, comp);
    std::uint64_t bitmask_lo =  _mm_movemask_epi8(mask_lo);
    std::uint64_t bitmask_hi =  _mm_movemask_epi8(mask_hi);
    return bitmask_hi << 32 & bitmask_lo;
}


inline __m128i byteswap(__m128i input, std::size_t n_symbols)
{
    __m128i shuffle_mask = _mm_loadu_si128(
            reinterpret_cast<const __m128i*>(shuffle_mask_lookup[n_symbols].data()));
    // swap order for all digits so the least significant digits come first in the vector
    __m128i shuffled = _mm_shuffle_epi8(input, shuffle_mask);
    return shuffled;
};


inline std::uint64_t parse_digits(const __m128i input, std::size_t n_digits)
{
    // byteswap and mask away non digits
    const __m128i swapped_input = sse41::byteswap(input, n_digits);

    // Substract 0 with saturation to convert chars to integers,
    // The sign symbols become zero.
    __m128i ascii0 = _mm_set1_epi8('0');
    __m128i t0 = _mm_subs_epu8(swapped_input, ascii0);

    // convert to 2-digit numbers
    __m128i mul_1_10 = _mm_set_epi8(10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1);
    __m128i t1 = _mm_maddubs_epi16(t0, mul_1_10);

    // convert to 4-digit numbers
    __m128i mul_1_100 = _mm_set_epi16(100, 1, 100, 1, 100, 1, 100, 1);

    __m128i t2 = _mm_madd_epi16(t1, mul_1_100);

    // convert from 32-bit into 16-bit element vector
    __m128i t3 = _mm_packus_epi32(t2, t2);

    // convert to 8-digit numbers
    __m128i mul_1_10000 = _mm_set_epi16(10000, 1, 10000, 1, 10000, 1, 10000, 1);

    __m128i t4 = _mm_madd_epi16(t3, mul_1_10000);

    std::uint64_t lo = _mm_extract_epi32(t4, 0);
    std::uint64_t hi = _mm_extract_epi32(t4, 1);

    auto power = power_of_10_lookup[std::max(0, static_cast<int>(n_digits) - 8)];
    std::uint64_t result = hi * power + lo;
    return result;
}
}

namespace bencode::detail {

constexpr from_chars_result
from_chars(const char* first, const char* last, std::uint32_t& value, implementation_tag<2> tag) noexcept
{
    const std::ptrdiff_t buffer_size = std::distance(first, last);

    if (buffer_size < 16) {
        return from_chars(first, last, value, implementation::serial);
    }

    const bool leading_zero = (*first == '0');
    std::uint64_t val = 0;
    std::ptrdiff_t position = 0;
    std::size_t n_digits;

    // load 32 characters at once
    __m128i data = _mm_loadu_si128(reinterpret_cast<const __m128i*>(first));
    // create a bitmask indicating the integers
    std::uint32_t digits_mask = sse41::decimal_digits_mask(data);
    n_digits = std::countr_one(digits_mask);

    if (n_digits == 0) [[unlikely]] {
        return {.ptr = first, .ec = parsing_errc::expected_digit};
    }

    if (n_digits > 10) [[unlikely]] {
        position += n_digits;
        // increment position to first non digit character
        for (; position < buffer_size && first[position] - '0' < 9; ++position);
        return {.ptr = first+position,
                .ec = parsing_errc::result_out_of_range};
    }

    val = sse41::parse_digits(data, n_digits);
    position += n_digits;

    from_chars_result res {.ptr = first+position, .ec = parsing_errc{}};

    if (n_digits == 10) [[unlikely]] {
        if (value < 1000000000) {
            res.ec = parsing_errc::result_out_of_range;
            return res;
        }
    }

    if (leading_zero && val != 0) [[unlikely]] {
        res.ec = parsing_errc::leading_zero;
        return res;
    }

    value = val;
    return {.ptr = first + position, .ec = parsing_errc{}};
}


constexpr from_chars_result
from_chars(const char* first, const char* last, std::uint64_t& value, implementation_tag<2> tag) noexcept
{
    const std::ptrdiff_t buffer_size = std::distance(first, last);

    if (buffer_size < 16) {
        return from_chars(first, last, value, implementation::serial);
    }

    const bool leading_zero = (*first == '0');
    std::uint64_t val = 0;
    std::ptrdiff_t position = 0;
    std::size_t n_digits;

    // load 16 characters at once
    __m128i data_lo = _mm_lddqu_si128(reinterpret_cast<const __m128i*>(first));
    // create a bitmask indicating the integers
    std::uint32_t digits_mask_lo = sse41::decimal_digits_mask(data_lo);
    n_digits = std::countr_one(digits_mask_lo);

    if (n_digits == 0) [[unlikely]] {
        return {.ptr = first, .ec = parsing_errc::expected_digit};
    }

    std::uint64_t val_lo = sse41::parse_digits(data_lo, n_digits);
    position += n_digits;

    if (n_digits >= 16) [[unlikely]] {
        __m128i data_hi = _mm_lddqu_si128(reinterpret_cast<const __m128i*>(first+16));
        std::uint32_t digits_mask_hi = sse41::decimal_digits_mask(data_hi);
        n_digits = std::countr_one(digits_mask_hi);
        position += n_digits;

        std::uint64_t val_hi = sse41::parse_digits(data_hi, n_digits);
        std::uint64_t base = sse41::power_of_10_lookup[std::max(0, static_cast<int>(n_digits))];

        if (!raise_and_add_safe(val_lo, base, val_hi)) {
            for (; position<buffer_size && first[position]-'0'<9; ++position);

            return {.ptr = first+position,
                    .ec = parsing_errc::result_out_of_range};
        }
    }

    from_chars_result res {.ptr = first + position, .ec = parsing_errc{}};

    if (leading_zero && val_lo != 0) [[unlikely]] {
        res.ec = parsing_errc::leading_zero;
        return res;
    }
    value = val_lo;
    return res;
}


template <std::signed_integral T>
constexpr from_chars_result from_chars(
        const char* first, const char* last, T& value, implementation_tag<2> tag)
{
    int sign = 1;
    const char* start = first;

    if (*first == '-') {
        sign = -1;
        ++first;

        if (first == last)  [[unlikely]] {
            return {first, parsing_errc::unexpected_eof};
        }
    }

    using UT = typename std::make_unsigned<std::remove_cvref_t<T>>::type;
    UT val = 0;

    from_chars_result res = from_chars(first, last, val, tag);

    if (res.ec != parsing_errc{}) [[unlikely]] {
        return res;
    }
    if (val == 0 && sign == -1) [[unlikely]] {
        res.ec = parsing_errc::negative_zero;
        return res;
    }

    T tmp;
    if (__builtin_mul_overflow(val, sign, &tmp)) {
        res.ec = parsing_errc::result_out_of_range;
    }
    else {
        value = tmp;
    }

    return res;
}


}