#pragma once

#include <string_view>
#include "bencode/detail/serialization_traits.hpp"

namespace bencode {

template <> struct serialization_traits<std::basic_string_view<char>>     : serializes_to_string {};
template <> struct serialization_traits<std::basic_string_view<wchar_t>>  : serializes_to_string {};
template <> struct serialization_traits<std::basic_string_view<char8_t>>  : serializes_to_string {};
template <> struct serialization_traits<std::basic_string_view<char16_t>> : serializes_to_string {};
template <> struct serialization_traits<std::basic_string_view<char32_t>> : serializes_to_string {};

}