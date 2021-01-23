#pragma once
#include <cstdint>

namespace bencode {

/// Pass safety options to a parser.
struct parser_options
{
    /// Maximum number of nested bencode objects.
    std::size_t recursion_limit = 1024;
    /// Maximum number of values to parse.
    std::size_t value_limit = 1UL << 20;
};

/// Wether to parse strings as a value type or a view type.
enum class string_parsing_mode : bool
{
    value = false,
    view = true
};

}
