#pragma once

#include <set>
#include "bencode/detail/serialization_traits.hpp"

namespace bencode {

template <typename T, typename Compare, typename Alloc>
struct serialization_traits<std::set<T, Compare, Alloc> >     : serializes_to_list {};

template <typename T, typename Compare, typename Alloc>
struct serialization_traits<std::multiset<T, Compare, Alloc> > : serializes_to_list {};

}
