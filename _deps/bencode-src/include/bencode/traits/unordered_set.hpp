#pragma once

#include <unordered_set>
#include "bencode/detail/serialization_traits.hpp"

namespace bencode {

template <typename T, typename Hash, typename Pred, typename Alloc>
struct serialization_traits<std::unordered_set<T, Hash, Pred, Alloc>>      : serializes_to_list {};

template <typename T, typename Hash, typename Pred, typename Alloc>
struct serialization_traits<std::unordered_multiset<T, Hash, Pred, Alloc>> : serializes_to_list {};

}