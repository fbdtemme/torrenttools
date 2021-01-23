#pragma once

#include <immintrin.h>

#include "bencode/detail/parser/parsing_error.hpp"
#include "bencode/detail/parser/from_chars.hpp"

/// @file Parsing routines for bencoded data for AVX2 instruction set architecture


namespace bencode::detail::avx2
{
using row = std::array<std::uint8_t, 32>;

inline constexpr auto shuffle_mask_lookup = std::array {
        row{0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
        row{0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
        row{1, 0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
        row{2, 1, 0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
        row{3, 2, 1, 0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
        row{4, 3, 2, 1, 0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
        row{5, 4, 3, 2, 1, 0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
        row{6, 5, 4, 3, 2, 1, 0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
        row{7, 6, 5, 4, 3, 2, 1, 0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
        row{8, 7, 6, 5, 4, 3, 2, 1, 0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
        row{9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
        row{10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
        row{11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0xff, 0xff, 0xff, 0xff, 0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
        row{12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0xff, 0xff, 0xff, 0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
        row{13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0xff, 0xff, 0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
        row{14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0xff, 0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
        row{15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0xff,
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
        row{15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0,
            0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
        row{15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 1,
            0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
        row{15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 2, 1,
            0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
        row{15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 3, 2, 1,
            0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
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


inline std::uint32_t decimal_digits_mask(__m256i input)
{
    // This moves codes of '0'..'9' to range 118, 127.
    // Then it is sufficient to compare the subtracted value for greater then 117.
    __m256i offset = _mm256_set1_epi8(70);
    __m256i comp = _mm256_set1_epi8(117);
    __m256i t0 = _mm256_add_epi8(input, offset);
    __m256i mask = _mm256_cmpgt_epi8(t0, comp);
    return _mm256_movemask_epi8(mask);
}

inline bool is_negative(__m256i input)
{
    char first_char = _mm256_extract_epi8(input, 0);
    return first_char == '-';
}

inline __m256i byteswap(__m256i input, std::size_t n_symbols)
{
    __m256i shuffle_mask = _mm256_loadu_si256(
        reinterpret_cast<const __m256i*>(shuffle_mask_lookup[n_symbols].data()));
        // swap order for all digits so the least significant digits come first in the vector
    __m256i shuffled = _mm256_shuffle_epi8(input, shuffle_mask);
    return shuffled;
};

inline std::uint64_t parse_digits(const __m256i input, std::size_t n_digits)
{
    // byteswap and mask away non digits
    const __m256i swapped_input = avx2::byteswap(input, n_digits);

    // Substract 0 with saturation to convert chars to integers,
    // The sign symbols become zero.
    __m256i ascii0 = _mm256_set1_epi8('0');
    __m256i t0 = _mm256_subs_epu8(swapped_input, ascii0);

    // convert to 2-digit numbers
    __m256i mul_1_10 = _mm256_set_epi8(
            10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1,
            10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1);

    __m256i t1 = _mm256_maddubs_epi16(t0, mul_1_10);

    // convert to 4-digit numbers
    __m256i mul_1_100 = _mm256_set_epi16(
            100, 1, 100, 1, 100, 1, 100, 1, 100, 1, 100, 1, 100, 1, 100, 1);

    __m256i t2 = _mm256_madd_epi16(t1, mul_1_100);

    // convert form 32-bit into 16-bit element vector
    __m256i t3 = _mm256_packus_epi32(t2, t2);

    // convert to 8-digit numbers
    __m256i mul_1_10000 = _mm256_set_epi16(
            10000, 1, 10000, 1, 10000, 1, 10000, 1, 10000, 1, 10000, 1, 10000, 1, 10000, 1);

    __m256i t4 = _mm256_madd_epi16(t3, mul_1_10000);

    std::uint64_t hi1 = _mm256_extract_epi32(t4, 0);
    std::uint64_t hi2 = _mm256_extract_epi32(t4, 1);
    std::uint64_t lo1 = _mm256_extract_epi32(t4, 4);

    std::uint64_t r1 = hi2 * 100000000 + hi1;
    std::uint64_t power = power_of_10_lookup[std::max(0, static_cast<int>(n_digits) - 16)];
    return r1 * power + lo1;
}
}

namespace bencode::detail {

template <std::unsigned_integral T>
constexpr from_chars_result
from_chars(const char* first, const char* last, T& value, implementation_tag<3> tag) noexcept
{
    const std::ptrdiff_t buffer_size = std::distance(first, last);

    if (buffer_size < 32) [[unlikely]] {
        return from_chars(first, last, value, implementation::serial);
    }

    bool negative = false;

    const bool leading_zero = (*first == '0');

    std::uint64_t val = 0;
    std::ptrdiff_t position = 0;
    std::size_t n_digits;

    // load 32 characters at once
    __m256i data = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(first));
    // create a bitmask indicating the integers
    std::uint32_t digits_mask = avx2::decimal_digits_mask(data);
    n_digits = std::countr_one(digits_mask);

    if (n_digits == 0) [[unlikely]] {
        return {.ptr = first, .ec = parsing_errc::expected_digit};
    }

    if (n_digits > 20) [[unlikely]] {
        position += n_digits;
        // increment position to first non digit character
        for (; position < buffer_size && first[position] - '0' < 9; ++position);
        return {.ptr = first+position,
                .ec = parsing_errc::result_out_of_range};
    }

    from_chars_result res {.ptr = first+position, .ec = parsing_errc{}};

    val = avx2::parse_digits(data, n_digits);

    if (n_digits == 20) [[unlikely]]  {
        if (val < 10000000000000000000ul) {
            return {.ptr = first + position,
                    .ec = parsing_errc::result_out_of_range};
        }
    }

    if (leading_zero && val != 0) [[unlikely]] {
        res.ec = parsing_errc::leading_zero;
        return res;
    }

    position += n_digits;
    value = val;
    return {.ptr = first + position, .ec = parsing_errc{}};
}


template <std::signed_integral T>
constexpr from_chars_result from_chars(
        const char* first, const char* last, T& value, implementation_tag<3> tag)
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