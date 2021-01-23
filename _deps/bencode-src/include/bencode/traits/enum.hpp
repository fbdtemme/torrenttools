#pragma once
#include <type_traits>
#include "bencode/detail/serialization_traits.hpp"

namespace bencode {

template <typename T>
    requires std::is_enum_v<T>
struct serialization_traits<T> : serializes_to_integer {};

};