#pragma once

#include <cstdint>
#include "bencode/detail/parser/parsing_error.hpp"
#include "bencode/detail/parser/helpers.hpp"

#include "bencode/detail/parser/implementation/from_chars_common.hpp"
#include "bencode/detail/parser/implementation/from_chars_serial.hpp"
#include "bencode/detail/parser/implementation/from_chars_swar.hpp"

#ifdef __SSE4_1__
#include "bencode/detail/parser/implementation/from_chars_sse41.hpp"
#endif

#ifdef __AVX2__
#include "bencode/detail/parser/implementation/from_chars_avx2.hpp"
#endif

namespace bencode::detail
{

template <std::integral T, std::size_t ImplementationIdx>
constexpr from_chars_result binteger_from_chars(
        const char* first, const char* last, T& value, implementation_tag<ImplementationIdx> tag)
{
    const char* start = first;
    from_chars_result res {.ptr = first, .ec = parsing_errc{}};

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
    from_chars_result from_chars_res = from_chars(first, last, val, tag);

    first = from_chars_res.ptr;

    // pass possible errors from parse_integer
    if (from_chars_res.ec != parsing_errc{}) [[unlikely]] {
        res = from_chars_res;
        return res;
    }

    // verify the integer is correctly terminated with the "e"
    if (first == last) [[unlikely]] {
        res.ec = parsing_errc::unexpected_eof;
        res.ptr = first;
        return res;
    }
    if (*first != symbol::end) [[unlikely]] {
        res.ec = parsing_errc::expected_end;
        res.ptr = first;
        return res;
    }

    ++first;
    res.ptr = first;
    value = val;
    return res;
}



template <std::size_t ImplementationIdx>
constexpr from_chars_result bstring_from_chars(const char* first, const char* last,
                                               std::size_t& offset, std::size_t& size,
                                               implementation_tag<ImplementationIdx> tag)
{
    auto start = first;
    from_chars_result res {.ptr = first, .ec = parsing_errc{}};

    if (first == last) [[unlikely]] {
        res.ec = parsing_errc::unexpected_eof;
        res.ptr = first;
        return res;
    }
    if (*first == '-') [[unlikely]] {
        res.ec = parsing_errc::negative_string_length;
        res.ptr = first;
        return res;
    }

    std::size_t size_val;
    const from_chars_result from_chars_res = from_chars(first, last, size_val, tag);

    if (from_chars_res.ec != parsing_errc{}) [[unlikely]] {
        res = from_chars_res;
        return res;
    }
    first = from_chars_res.ptr;

    if (first == last) [[unlikely]] {
        res.ec = parsing_errc::unexpected_eof;
        res.ptr = first;
        return res;
    }
    if (*first != ':') [[unlikely]] {
        res.ec = parsing_errc::expected_colon;
        res.ptr = first;
        return res;
    }

    ++first;
    offset = std::distance(start, first);
    std::advance(first, size_val);

    if (first > last) [[unlikely]] {
        res.ec = parsing_errc::unexpected_eof;
        res.ptr = first;
        return res;
    }

    size = size_val;
    res.ptr = first;
    return res;
}


template <std::size_t ImplementationIdx>
constexpr from_chars_result bstring_from_chars(const char* first, const char* last,
        std::string_view& sv,
        implementation_tag<ImplementationIdx> tag)
{
    std::size_t offset;
    std::size_t size;

    auto res = bstring_from_chars(first, last, offset, size, tag);
    if (res.ec != parsing_errc{}) [[unlikely]] {
        return res;
    }
    sv = std::string_view(std::next(first, offset), std::next(first, offset+size));
    return res;
}

}

