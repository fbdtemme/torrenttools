#pragma once

#include <string>

#include "bencode/detail/events/events.hpp"
#include "bencode/detail/events/encode_to.hpp"
#include "bencode/detail/concepts.hpp"

/// @file

namespace bencode {


/// Write bencoded representation of value to a string.
/// @param value the value to encode
/// @returns a string with the bencoded representation of value.
template <event_producer T>
inline std::string encode(const T& value)
{
    std::string s {};
    auto consumer = bencode::events::encode_to(std::back_inserter(s));
    connect(consumer, value);
    return s;
}


/// Writes the bencoded representation of value to os.
/// @param os an output stream to write to
/// @param value the value to encode
template <event_producer T>
inline void encode_to(std::ostream& os, const T& value)
{
    auto consumer = bencode::events::encode_to(os);
    connect(consumer, value);
}

/// Writes the bencoded representation of value to it.
/// @param it output iterator to write to
/// @param value the value to encode
template <std::output_iterator<char> OutputIt, event_producer T>
inline void encode_to(OutputIt it, const T& value)
{
    auto consumer = bencode::events::encode_to(it);
    connect(consumer, value);
}

} // namespace bencode

