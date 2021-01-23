/*===----------------------------------------------------------------------===*
 * itoa_ljust_impl.h - Fast integer to ascii decimal conversion
 *
 * This file is meant to be included in only one .cpp file/compilation unit.
 * Uses recursive function templates, compile with -O3 for best performance.
 *
 * The implementation strives to perform well with random input values
 * where CPU branch prediction becomes ineffective:
 *
 *   1 ) reduce the number of conditional branches used to determine
 *       the number of digits and use uninterrupted sequence of
 *       instructions to generate multiple digits, this was inspired by
 *       the implementation of FastUInt32ToBufferLeft in
 *           https://github.com/google/protobuf/blob/master/
 *                 src/google/protobuf/stubs/strutil.cc
 *
 *   2 ) avoid branches altogether by allowing overwriting of characters
 *       in the output buffer when the difference is only one character
 *          a) minus sign
 *          b) alignment to even # digits
 *
 *   3 ) use hints to the compiler to indicate which conditional branches
 *       are likely to be taken so the compiler arranges the likely
 *       case to be the fallthrough, branch not taken
 *
 * Other Performance considerations
 *
 *   4 ) use a lookup table to convert binary numbers 0..99 into 2 characters
 *       This technique is used by all fast implementations.
 *       See Andrei Alexandrescu's engineering notes
 *           https://www.facebook.com/notes/facebook-engineering/
 *                 three-optimization-tips-for-c/10151361643253920/
 *
 *   5 ) use memcpy to store 2 digits at a time - most compilers treat
 *       memcpy as a builtin/intrinsic, this lets the compiler
 *       generate a 2-byte store instruction in platforms that support
 *       unaligned access
 *
 *   6 ) use explicit multiplicative inverse to perform integer division
 *       See Terje Mathisen's algoritm in Agner Fog's
 *           http://www.agner.org/optimize/optimizing_assembly.pdf
 *       By exploiting knowledge of the restricted domain of the dividend
 *       the multiplicative inverse factor is smaller and can fit in the
 *       immediate operand of x86 multiply instructions, resulting in
 *       fewer instructions
 *
 *   7 ) inline the recursive call to FastUInt64ToBufferLeft in the original
 *       Google Protocol Buffers, as in itoa-benchmark/src/unrolledlut.cpp
 *       by nyronium@genthree.io
 *
 * Correctness considerations
 *
 *   8 ) Avoids unary minus of signed types - undefined behavior if bvalue
 *       is INT_MIN in platforms using two's complement representation
 *
 *===----------------------------------------------------------------------===*
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016-2017 Arturo Martin-de-Nicolas
 * arturomdn@gmail.com
 * https://github.com/amdn/itoa_ljust/
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 *     The above copyright notice and this permission notice shall be included
 *     in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *===----------------------------------------------------------------------===*/

// taken from: https://github.com/miloyip/itoa-benchmark
// the original source at https://github.com/amdn/itoa_ljust/ seems to be out
// of date.

// * Adapted to C++17 for clearer code
// * Added constexpr execution.
// * removed writing null byte;
//
// Performance is almost identical to the original version.

#pragma once

#include <concepts>
#include <array>
#include <type_traits>
#include <cstring>      // std::memcpy
#include <stdexcept>
#include <iostream>

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wattributes"
#endif

namespace inttostr {

namespace detail {


/// generate a two digits lookup table
constexpr std::array<char, 200> generate_table() noexcept
{
    std::array<char, 200> table{};
    std::size_t idx = 0;

    for (char i = '0'; i <= '9'; ++i) {
        for (char j = '0'; j <= '9'; ++j, ++idx) {
            table[idx] = i;
            table[++idx] = j;
        }
    }
    return table;
};

struct digits_lookup { /* Note 4 */
    constexpr digits_lookup() = default;

    constexpr char const* operator()(uint32_t d) const noexcept
    { return &table_[2 * d]; }

private:
    static constexpr std::array<char, 200> table_ = generate_table();
};

constexpr digits_lookup digits{};

constexpr uint32_t p10(uint32_t e)
{
    return e ? 10 * p10(e-1) : 1;
}

template<uint32_t E>
constexpr uint32_t q10(uint32_t u)        /* Note 6 */
{
    if constexpr (E == 0) { return u; }
    if constexpr (E == 2) { return (static_cast<uint64_t>(u)       * 5243U)       >> 19U; } // u < 10^4
    if constexpr (E == 4) { return ((1 + static_cast<uint64_t>(u)) * 858993U)     >> 33U; } // u < 10^6
    if constexpr (E == 6) { return ((1 + static_cast<uint64_t>(u)) * 8796093U)    >> 43U; } // u < 10^8
    if constexpr (E == 8) { return (static_cast<uint64_t>(u)       * 1441151881U) >> 57U; } // u < 2^32
    else
        throw std::logic_error(std::to_string(E));
};

template<uint32_t E>
constexpr uint64_t q10(uint64_t u)
{
    return u / p10(E);
}

template<typename T, uint32_t E, typename U = T>
struct QR {
    const T q;
    const T r;

    constexpr explicit QR(U u)
            :q(q10<E>(u)), r(u-q * p10(E)) { }
};

template<uint32_t E>
constexpr char* cvt(char* out, uint32_t u) noexcept
{
    if constexpr (E == 0) {
//        *out = '\0';
        return out;
    }
    else if constexpr (E % 2) {
        QR<uint32_t, E-1> d{u};
        char const* src = digits(d.q);
        *out = *src++;
        out += d.q > 9; /* Note 2b */
        *out++ = *src;
        return cvt<E-1>(out, d.r);
    }
    else if constexpr (!(E % 2)) {
        QR<uint32_t, E-2> d{u};
        std::memcpy(out, digits(d.q), 2); /* Note 5 */
        return cvt<E-2>(out+2, d.r);
    }
    else {
        throw std::logic_error("this should be unreachable code!");
    };
}

/* Note 1 */
constexpr char* to_dec(char* out, uint32_t u)
{
    if (u >= p10(8)) [[unlikely]] {
        return cvt<9>(out, u);
    }
    else if (u < p10(2)) [[likely]] {
        return cvt<1>(out, u);
    }
    else if (u < p10(4)) [[likely]] {
        return cvt<3>(out, u);
    }
    else if (u < p10(6)) [[likely]] {
        return cvt<5>(out, u);
    }
    else
        return cvt<7>(out, u);
}

/// Note 7
constexpr char* to_dec(char* out, uint64_t u)
{
    if (auto low = static_cast<uint32_t>(u); low == u) [[likely]] {
        return to_dec(out, low);
    }

    QR<uint64_t, 8> mid{u};
    auto mid32 = static_cast<uint32_t>(mid.q);

    if (mid32 == mid.q) [[likely]] {
        out = to_dec(out, mid32);
        return cvt<8>(out, static_cast<uint32_t>(mid.r));
    }
    else {
        QR<uint32_t, 8, uint64_t> d{mid.q};
        out = d.q < p10(2) ? cvt<1>(out, d.q) : cvt<3>(out, d.q);
        out = cvt<8>(out, d.r);
        return cvt<8>(out, static_cast<uint32_t>(mid.r));
    }
}

template<std::signed_integral T>
static constexpr char* to_dec(char* out, T v)
{
    using U = std::make_unsigned_t<T>;
    U mask = v < 0 ? ~U{0} : 0;                 /* Note 2a */
    U u = (2 * static_cast<U>(v) & ~mask)-static_cast<U>(v);      /* Note 8  */
    *out = '-';
    return to_dec(out + (mask & 1), u);
}

} // namespace detail


template <std::integral T>
constexpr char* itoa(T v, char* out) {
    auto end = detail::to_dec(out, v);
    *end++ = '\0';
    return end;
}

constexpr char* u32toa(uint32_t v, char* out) { return itoa(v, out); }
constexpr char* u64toa(uint64_t v, char* out) { return itoa(v, out); }
constexpr char* i32toa( int32_t v, char* out) { return itoa(v, out); }
constexpr char* i64toa( int64_t v, char* out) { return itoa(v, out); }


template <typename T>
constexpr std::size_t to_buffer(char* first, T v)
{
    char* last = detail::to_dec(first, v);
    return std::distance(first, last);
}

template <std::integral T>
void to_stream(std::ostream& os, T v)
{
    std::array<char, 24> buffer{};   // 64 bit fits in 20 chars, 24 bit for alignment;
    char* end = detail::to_dec(buffer.data(), v);
    size_t size = std::distance(buffer.begin(), end);
    os.write(buffer.data(), size);
}


template <std::integral T>
std::string to_string(T v)
{
    // 32 bit values should fit in stack storage of std::string (11 digits + zero)
    // write directly to data pointer
    if constexpr (sizeof(T) <= 32) {
        std::string s(11, '\0');
        char* end = detail::to_dec(s.data(), v);
        s.resize(std::distance(s.data(), end));
        return s;
    }
    else {
        std::array<char, 20> buffer{}; // 64 bit fits in 20 chars
        char* end = itoa(v, buffer.data());
        return std::string(buffer.data(), end);
    }
}


} // namespace inttostr

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
