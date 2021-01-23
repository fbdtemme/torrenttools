#pragma once
#include <forward_list>
#include "bencode/detail/serialization_traits.hpp"

namespace bencode {

template <typename T, typename Alloc>
struct serialization_traits<std::forward_list<T, Alloc>> : serializes_to_list {};

}