#pragma once

#include <string>
#include "bencode/detail/serialization_traits.hpp"

namespace bencode {

template <typename Traits, typename Alloc>
struct serialization_traits<std::basic_string<char, Traits, Alloc>>     : serializes_to_string {};

template <typename Traits, typename Alloc>
struct serialization_traits<std::basic_string<wchar_t, Traits, Alloc>>  : serializes_to_string {};

template <typename Traits, typename Alloc>
struct serialization_traits<std::basic_string<char8_t, Traits, Alloc>>  : serializes_to_string {};

template <typename Traits, typename Alloc>
struct serialization_traits<std::basic_string<char16_t, Traits, Alloc>> : serializes_to_string {};

template <typename Traits, typename Alloc>
struct serialization_traits<std::basic_string<char32_t, Traits, Alloc>> : serializes_to_string {};

}