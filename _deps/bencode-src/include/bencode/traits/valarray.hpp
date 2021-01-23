#pragma once

#include <valarray>
#include "bencode/detail/serialization_traits.hpp"

namespace bencode {

template <> struct serialization_traits<std::valarray<char>>     : serializes_to_string {};
template <> struct serialization_traits<std::valarray<wchar_t>>  : serializes_to_string {};
template <> struct serialization_traits<std::valarray<char8_t>>  : serializes_to_string {};
template <> struct serialization_traits<std::valarray<char16_t>> : serializes_to_string {};
template <> struct serialization_traits<std::valarray<char32_t>> : serializes_to_string {};

template <typename T>
struct serialization_traits<std::valarray<T>> : serializes_to_list {};

}