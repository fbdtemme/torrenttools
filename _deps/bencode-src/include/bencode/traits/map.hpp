#pragma once

#include <map>
#include "bencode/detail/serialization_traits.hpp"
#include "bencode/traits/pair.hpp"

namespace bencode {

template <typename Key, typename T, typename Alloc>
struct serialization_traits<std::map<Key, T, std::less<Key>, Alloc> >     :
        serializes_to_dict<dict_key_order::sorted> {};

template <typename Key, typename T, typename Alloc>
struct serialization_traits<std::multimap<Key, T, std::less<Key>, Alloc> > :
        serializes_to_dict<dict_key_order::sorted> {};

template <typename Key, typename T, typename Alloc>
struct serialization_traits<std::map<Key, T, std::less<>, Alloc> >     :
        serializes_to_dict<dict_key_order::sorted> {};

template <typename Key, typename T, typename Alloc>
struct serialization_traits<std::multimap<Key, T, std::less<>, Alloc> > :
        serializes_to_dict<dict_key_order::sorted> {};

}
