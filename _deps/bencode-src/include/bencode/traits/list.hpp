#pragma once

#include <list>
#include "bencode/detail/serialization_traits.hpp"

namespace bencode {

template <typename T, typename Alloc>
struct serialization_traits<std::list<T, Alloc>> : serializes_to_list {};

}