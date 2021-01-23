#pragma once

#include <array>
#include "bencode/detail/serialization_traits.hpp"

namespace bencode {

template <typename Alloc> struct serialization_traits<std::vector<char,      Alloc>> : serializes_to_string {};
template <typename Alloc> struct serialization_traits<std::vector<wchar_t,   Alloc>> : serializes_to_string {};
template <typename Alloc> struct serialization_traits<std::vector<char8_t,   Alloc>> : serializes_to_string {};
template <typename Alloc> struct serialization_traits<std::vector<char16_t,  Alloc>> : serializes_to_string {};
template <typename Alloc> struct serialization_traits<std::vector<char32_t,  Alloc>> : serializes_to_string {};
template <typename Alloc> struct serialization_traits<std::vector<std::byte, Alloc>> : serializes_to_string {};

template <typename T, typename Alloc>
struct serialization_traits<std::vector<T, Alloc>> : serializes_to_list {};

}