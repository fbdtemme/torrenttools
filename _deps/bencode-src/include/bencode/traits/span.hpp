#pragma once

#include <span>
#include "bencode/detail/serialization_traits.hpp"

namespace bencode {

template <std::size_t Extent>
struct serialization_traits<std::span<char, Extent>>   : serializes_to_string {};

template <std::size_t Extent>
struct serialization_traits<std::span<const char, Extent>>   : serializes_to_string {};

template <std::size_t Extent>
struct serialization_traits<std::span<std::byte, Extent>>   : serializes_to_string {};

template <std::size_t Extent>
struct serialization_traits<std::span<const std::byte, Extent>>   : serializes_to_string {};

template <serializable T, std::size_t Extent>
struct serialization_traits<std::span<T, Extent>>   : serializes_to_list {};

template <serializable T, std::size_t Extent>
struct serialization_traits<std::span<const T, Extent>>   : serializes_to_list {};

}