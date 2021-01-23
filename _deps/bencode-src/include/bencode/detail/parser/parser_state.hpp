#pragma once
#include <cstdint>

namespace bencode::detail {
/// Enum type of the current parse context in nested objects for list and dicts.
enum class parser_state : std::uint8_t {
    expect_value             = 0,
    expect_list_value        = 1,
    expect_dict_key          = 2,
    expect_dict_value        = 4,
};
}
