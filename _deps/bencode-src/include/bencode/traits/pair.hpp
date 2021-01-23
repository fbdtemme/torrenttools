#pragma once

#include <utility>
#include "bencode/detail/serialization_traits.hpp"

namespace bencode {

template <typename U, typename V>
struct serialization_traits<std::pair<U, V>> : serializes_to_list {};

}