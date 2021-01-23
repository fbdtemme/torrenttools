#pragma once

#include <string>
#include <gsl-lite/gsl-lite.hpp>

#include <ranges>

#include <bencode/detail/parser/push_parser.hpp>
#include <bencode/detail/events/to_bvalue.hpp>
#include <bencode/detail/events/encode_to.hpp>

/// @file


namespace bencode {

namespace rng = std::ranges;


/// Reads bencoded data from the input stream and decodes it to a basic_bvalue.
/// @tparam Policy the policy template argument for basic_bvalue,
///     defaults to default_bvalue_policy.
/// @param is the input stream to read from
/// @return the decoded data as a basic_bvalue instantiation.
/// @throw parse_error when the bencoded data is malformed.
template <typename Policy = default_bvalue_policy>
inline basic_bvalue<Policy> decode_value(std::istream& is)
{
    auto consumer = bencode::events::to_bvalue<Policy>{};
    push_parser<string_parsing_mode::value, std::istreambuf_iterator<char>> parser {};

    auto range = rng::subrange(std::istreambuf_iterator<char>{is},
                               std::istreambuf_iterator<char>{});
    bool success = parser.parse(consumer, range);
    if (!success) {
        throw parser.error();
    }
    return consumer.value();
}

/// Reads bencoded data from a std::string_view and decodes it to a it to a basic_bvalue.
/// @tparam Policy the policy template argument for basic_bvalue,
///     defaults to default_bvalue_policy.
/// @param sv the string_view to read from
/// @return the decoded data as a basic_bvalue instantiation.
/// @throws parse_error when the bencoded data is malformed.
template <typename Policy = default_bvalue_policy>
inline basic_bvalue<Policy> decode_value(std::string_view sv)
{
    auto consumer = bencode::events::to_bvalue<Policy>{};
    bencode::push_parser parser{};
    if (!parser.parse(consumer, sv))
        throw parser.error();
    return consumer.value();
}

/// Reads bencoded data from a range and decodes it to a it basic_bvalue.
/// @tparam Policy the policy template argument for basic_bvalue,
///     defaults to default_bvalue_policy.
/// @param range the input range to read from
/// @return the decoded data as a basic_bvalue instantiation.
/// @throws parse_error when the bencoded data is malformed.
template <typename Policy = default_bvalue_policy, rng::input_range Rng>
/// \cond CONCEPTS
    requires std::convertible_to<rng::range_value_t<const Rng&>, char> &&
            (!std::convertible_to<const Rng&, std::string_view>)
/// \endcond
inline basic_bvalue<Policy> decode_value(const Rng& range)
{
    auto consumer = bencode::events::to_bvalue<Policy>{};
    bencode::push_parser<string_parsing_mode::value,
            rng::iterator_t<const Rng&>,
            rng::sentinel_t<const Rng&>> parser{};
        if (!parser.parse(consumer, range))
            throw parser.error();
        return consumer.value();
//    }
}

/// Reads bencoded data from a pair of iterators to a basic_bvalue.
/// @tparam Policy the policy template argument for basic_bvalue,
///     defaults to default_bvalue_policy.
/// @param first an iterator pointing to the start of the bencoded data
/// @param last an iterator pointing one past the end of the bencoded data
/// @return the decoded data as a basic_bvalue instantiation.
/// @throws parse_error when the bencoded data is malformed.
template <typename Policy = default_bvalue_policy, std::input_iterator InputIterator>
/// \cond CONCEPTS
    requires std::same_as<std::iter_value_t<InputIterator>, char>
/// \endcond
inline basic_bvalue<Policy> decode_value(InputIterator first, InputIterator last)
{
    auto consumer = bencode::events::to_bvalue<Policy>{};
    bencode::push_parser<string_parsing_mode::value, InputIterator, InputIterator> parser{};
    if (!parser.parse(rng::subrange(first, last), consumer))
        throw parser.error();
    return consumer.value();
}

/// Read a basic_bvalue from an input stream.
/// @param is the input stream to read data from
/// @returns A basic_bvalue instance with data from is.
template <typename Policy>
inline std::istream& operator>>(std::istream& is, basic_bvalue<Policy>& value)
{
    value = decode_value<Policy>(is);
    return is;
}

}

namespace bencode::literals {

/// Construct a bvalue from a bencoded string.
inline bencode::bvalue operator""_bvalue(const char* str, std::size_t len)
{
    return bencode::decode_value(std::string_view(str, len));
}

}

