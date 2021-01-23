#pragma once

#include <span>
#include <bit>
#include <array>
#include <optional>
#include <numeric>
#include <filesystem>

#if defined(_WIN32)
#include <windows.h>
#endif

namespace dottorrent::detail {

/// helper for static_assert

template <typename... Args>
struct always_false : std::false_type {};

template <typename... Args>
constexpr bool always_false_v = always_false<Args...>::value;

//
//inline constexpr std::array<char, 16> int_to_hex_table = {
//        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
//        'a', 'b', 'c', 'd', 'e', 'f'
//};
//
//inline constexpr auto generate_hex_to_int_table() noexcept
//{
//    std::array<unsigned char, 256> data{};
//
//    // TODO: [C++20] Change to constexpr std::fill
//    for (std::size_t i = 0; i < 256; ++i) {
//        data[i] = std::numeric_limits<unsigned char>::max();
//    }
//    for (signed char a = '0'; a <= '9'; ++a) { data[a] = (a-'0'); }
//    for (signed char a = 'a'; a <= 'f'; ++a) { data[a] = (a-'a'+10); }
//    for (signed char a = 'A'; a <= 'F'; ++a) { data[a] = (a-'A'+10); }
//    return data;
//}
//
//struct hex_to_int_lookup_table
//{
//    constexpr hex_to_int_lookup_table() = default;
//    constexpr unsigned char operator()(char i) const noexcept
//    { return table_[i]; }
//
//private:
//    static constexpr std::array<unsigned char, 256> table_ = generate_hex_to_int_table();
//};
//
//inline constexpr auto hex_to_int = hex_to_int_lookup_table{};
//
//inline constexpr std::optional<std::byte> hex_to_byte(char a, char b)
//{
//    unsigned char upper = hex_to_int(a);
//    unsigned char lower = hex_to_int(b);
//    if ((upper & lower) == 0xFF) [[unlikely]] return std::nullopt;
//    return std::byte((upper << 4u) + lower);
//}
//
//inline constexpr std::optional<std::byte> hex_to_byte(std::string_view hex_pair)
//{
////    Expects(hex_pair.size() == 2);
//    return hex_to_byte(hex_pair[0], hex_pair[1]);
//}
//
//template <std::size_t N>
//inline constexpr std::optional<std::array<std::byte, N>>
//parse_hexdigest(std::string_view hexdigest)
//{
////    Expects(hexdigest.size() == 2 * N);
//    std::array<std::byte, N> data{};
//
//    for (size_t i = 0; i < hexdigest.size() / 2; ++i) {
//        auto byte_value = hex_to_byte(hexdigest.substr(2 * i, 2));
//        if (!byte_value) [[unlikely]] return std::nullopt;
//        data[i] = *byte_value;
//    }
//    return data;
//}
//
//template <typename OutputIt>
//inline constexpr OutputIt parse_hexdigest_to(OutputIt out, std::string_view hexdigest)
//{
//    for (size_t i = 0; i < hexdigest.size() / 2; ++i) {
//        auto byte_value = hex_to_byte(hexdigest.substr(2 * i, 2));
//        if (!byte_value) [[unlikely]] return out;
//        *out++ = *byte_value;
//    }
//    return out;
//}
//
//inline std::string make_hex_string(std::span<const std::byte> data)
//{
//    std::string s(data.size() * 2, ' ');
//    for (size_t i = 0; i < s.size(); ++i) {
//        auto c = static_cast<const uint8_t>(data[i]);
//        s[2 * i] = detail::int_to_hex_table[(c & 0xF0u)>>4u];
//        s[2 * i+1] = detail::int_to_hex_table[c & 0x0Fu];
//    }
//    return s;
//}


constexpr unsigned long long log2_floor(unsigned long long x) noexcept {
    return x ? 63 - std::countl_zero(x) : 0;
}

constexpr unsigned long long log2_ceil(unsigned long long x) noexcept {
    return log2_floor(2*x - 1);
}


struct discard_output_iterator
{
    using iterator_category = std::output_iterator_tag;
    using value_type = void;
    using difference_type = std::ptrdiff_t;
    using pointer = void*;
    using reference = void;

    constexpr discard_output_iterator() = default;

    /* no-op assignment */
    template<typename T>
    constexpr discard_output_iterator& operator=(T const&)
    { return *this; }

    constexpr discard_output_iterator& operator++()
    { return *this; }

    constexpr const discard_output_iterator operator++(int)
    { return {}; }

    constexpr discard_output_iterator& operator*()
    { return *this; }
};


inline bool is_hidden_file([[maybe_unused]] const std::filesystem::path& path) {
#if defined(_WIN32)
    int attr = GetFileAttributesW(path.c_str());
    return (attr & FILE_ATTRIBUTE_HIDDEN == 0);
#else
    return false;
#endif
}

} // namespace bencode::detail