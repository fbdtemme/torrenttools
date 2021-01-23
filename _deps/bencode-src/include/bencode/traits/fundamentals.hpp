#pragma once

#include <cstdint>

#include <bencode/detail/serialization_traits.hpp>

namespace bencode {

template <> struct serialization_traits<bool>      : bencode::serializes_to_integer {};
template <> struct serialization_traits<short>              : bencode::serializes_to_integer {};
template <> struct serialization_traits<int>                : bencode::serializes_to_integer {};
template <> struct serialization_traits<long>               : bencode::serializes_to_integer {};
template <> struct serialization_traits<long long>          : bencode::serializes_to_integer {};
template <> struct serialization_traits<unsigned short>     : bencode::serializes_to_integer {};
template <> struct serialization_traits<unsigned int>       : bencode::serializes_to_integer {};
template <> struct serialization_traits<unsigned long>      : bencode::serializes_to_integer {};
template <> struct serialization_traits<unsigned long long> : bencode::serializes_to_integer {};

template <> struct serialization_traits<signed char>        : bencode::serializes_to_integer {};
template <> struct serialization_traits<unsigned char>      : bencode::serializes_to_integer {};
template <> struct serialization_traits<char>               : bencode::serializes_to_integer {};
template <> struct serialization_traits<wchar_t>            : bencode::serializes_to_integer {};
template <> struct serialization_traits<char8_t>            : bencode::serializes_to_integer {};
template <> struct serialization_traits<char16_t>           : bencode::serializes_to_integer {};
template <> struct serialization_traits<char32_t>           : bencode::serializes_to_integer {};

template <> struct serialization_traits<const char*>         : bencode::serializes_to_string {};
template <> struct serialization_traits<const wchar_t*>      : bencode::serializes_to_string {};
template <> struct serialization_traits<const char8_t*>      : bencode::serializes_to_string {};
template <> struct serialization_traits<const char16_t*>     : bencode::serializes_to_string {};
template <> struct serialization_traits<const char32_t*>     : bencode::serializes_to_string {};

template <std::size_t N> struct serialization_traits<char[N]>           : bencode::serializes_to_string {};
template <std::size_t N> struct serialization_traits<wchar_t[N]>        : bencode::serializes_to_string {};
template <std::size_t N> struct serialization_traits<char8_t[N]>        : bencode::serializes_to_string {};
template <std::size_t N> struct serialization_traits<char16_t[N]>       : bencode::serializes_to_string {};
template <std::size_t N> struct serialization_traits<char32_t[N]>       : bencode::serializes_to_string {};
template <std::size_t N> struct serialization_traits<const char[N]>     : bencode::serializes_to_string {};
template <std::size_t N> struct serialization_traits<const wchar_t[N]>  : bencode::serializes_to_string {};
template <std::size_t N> struct serialization_traits<const char8_t[N]>  : bencode::serializes_to_string {};
template <std::size_t N> struct serialization_traits<const char16_t[N]> : bencode::serializes_to_string {};
template <std::size_t N> struct serialization_traits<const char32_t[N]> : bencode::serializes_to_string {};

template <> struct serialization_traits<std::byte>                       : serializes_to_integer {};
template <std::size_t N> struct serialization_traits<std::byte[N]>       : serializes_to_string  {};
template <std::size_t N> struct serialization_traits<const std::byte[N]> : serializes_to_string  {};


template <bencode::serializable T>
struct serialization_traits<T*> : serialization_traits<T>
{
    static constexpr auto is_pointer = true;
};

} // namespace bencode