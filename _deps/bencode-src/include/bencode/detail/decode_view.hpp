#pragma once

#include <string>
#include <gsl-lite/gsl-lite.hpp>

#include <ranges>

#include <bencode/bencode.hpp>
#include <bencode/detail/parser/descriptor_parser.hpp>


namespace bencode {



/// Reads bencoded data from a std::string_view and decodes it to a it to a descriptor_table.
/// @param view the bencoded data
/// @return the decoded data as a descriptor table.
/// @throw parse_error when the bencoded data is malformed.
inline descriptor_table decode_view(std::string_view view)
{
    bencode::descriptor_parser parser{};
    auto r = parser.parse(view);
    if (!r) { throw parser.error(); }
    return std::move(*r);
}


/// Reads bencoded data from a contiguous range and decodes it to a it to a descriptor_table.
/// @param range the range of bencoded data
/// @returns the decoded data as a descriptor_table.
/// @throws parse_error when the bencoded data is malformed.
template <rng::contiguous_range Rng>
/// \cond CONCEPTS
    requires std::same_as<rng::range_value_t<Rng>, char>
/// \endcond
inline descriptor_table decode_view(Rng&& range)
{
    bencode::descriptor_parser parser{};
    auto r = parser.parse(std::forward<Rng>(range));
    if (!r) { throw parser.error(); }
    return std::move(*r);
}


/// Reads bencoded data from a pair of iterators and decodes it to a descriptor_table.
/// @param first, last the range of characters to decode
/// @returns the decoded data as a descriptor table.
/// @throws parse_error when the bencoded data is malformed.
template <std::input_iterator InputIterator>
/// \cond CONCEPTS
   requires std::same_as<typename std::iterator_traits<InputIterator>::value_type, char>
/// \endcond
inline descriptor_table decode_view(InputIterator first, InputIterator last)
{
    bencode::descriptor_parser parser{};
    auto r = parser.parse(rng::subrange(first, last));
    if (!r) { throw r.error(); }
    return std::move(*r);
}

}