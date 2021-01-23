#pragma once

#include <array>
#include <bencode/detail/serialization_traits.hpp>
namespace bencode {

template <std::size_t N> struct serialization_traits<std::array<char,      N>> : serializes_to_string {} ;
template <std::size_t N> struct serialization_traits<std::array<wchar_t,   N>> : serializes_to_string {} ;
template <std::size_t N> struct serialization_traits<std::array<char8_t,   N>> : serializes_to_string {} ;
template <std::size_t N> struct serialization_traits<std::array<char16_t,  N>> : serializes_to_string {} ;
template <std::size_t N> struct serialization_traits<std::array<char32_t,  N>> : serializes_to_string {} ;
template <std::size_t N> struct serialization_traits<std::array<std::byte, N>> : serializes_to_string {} ;


template <typename T, std::size_t N>
struct serialization_traits<std::array<T, N>> : serializes_to_list {} ;

}