#pragma once

#include <span>
#include <array>
#include <optional>
#include <numeric>
#include <filesystem>
#include <charconv>

namespace dottorrent {

namespace detail {

inline constexpr std::array<char, 16> int_to_hex_table = {
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
        'a', 'b', 'c', 'd', 'e', 'f'
};

inline constexpr auto generate_hex_to_int_table() noexcept
{
    std::array<unsigned char, 256> data{};

    // TODO: [C++20] Change to constexpr std::fill
    for (std::size_t i = 0; i < 256; ++i) {
        data[i] = std::numeric_limits<unsigned char>::max();
    }
    for (signed char a = '0'; a <= '9'; ++a) { data[a] = (a-'0'); }
    for (signed char a = 'a'; a <= 'f'; ++a) { data[a] = (a-'a'+10); }
    for (signed char a = 'A'; a <= 'F'; ++a) { data[a] = (a-'A'+10); }
    return data;
}

struct hex_to_int_lookup_table
{
    constexpr hex_to_int_lookup_table() = default;
    constexpr unsigned char operator()(char i) const noexcept
    { return table_[i]; }

private:
    static constexpr std::array<unsigned char, 256> table_ = generate_hex_to_int_table();
};

inline constexpr auto hex_to_int = hex_to_int_lookup_table{};

inline constexpr bool hex_to_byte(char a, char b, std::byte& out)
{
    unsigned char upper = hex_to_int(a);
    unsigned char lower = hex_to_int(b);
    if ((upper & lower) == 0xFF) [[unlikely]] return false;
    out = std::byte((upper << 4u) + lower);
    return true;
}

inline constexpr bool hex_to_byte(std::string_view hex_pair, std::byte& out)
{
    return hex_to_byte(hex_pair[0], hex_pair[1], out);
}

// write hexademil representation of a byte to out
inline void constexpr byte_to_hex(char* __restrict out, std::byte c)
{
    auto v = to_integer<unsigned char>(c);
    out[0] = detail::int_to_hex_table[(v & 0xf0) >> 4u];
    out[1] = detail::int_to_hex_table[v & 0x0f];
}

}

template <std::output_iterator<std::byte> OutputIt>
inline constexpr void from_hexadecimal_string(OutputIt out, std::string_view hex_string)
{
    const char* it = hex_string.begin();
    while (it != hex_string.end()) {
        char v;
        auto r = std::from_chars(it, it+2, v, /*base*/ 16);

        if (r.ec != std::errc{}) [[unlikely]] {
            throw std::system_error(std::make_error_code(r.ec));
        }
        it = r.ptr;
        *out = std::byte(v);
    }
}

template <std::size_t N>
inline std::optional<std::array<std::byte, N>>
constexpr from_hexadecimal_string(std::string_view hexdigest)
{
    std::array<std::byte, N> data{};
    if (hexdigest.size()/2 != N) return std::nullopt;

    std::byte byte_value;
    for (size_t i = 0; i < hexdigest.size() / 2; ++i) {
        auto succes = detail::hex_to_byte(hexdigest.substr(2 * i, 2), byte_value);
        if (!succes) [[unlikely]] return std::nullopt;
        data[i] = byte_value;
    }
    return data;
}

inline std::string to_hexadecimal_string(std::span<const std::byte> data)
{
    std::string s(data.size() * 2, ' ');

    char* out = s.data();
    for (const auto c : data) {
        detail::byte_to_hex(out, c);
        std::advance(out, 2);
    }
    return s;
}

template <std::output_iterator<char> OutputIt>
inline constexpr char* to_hexadecimal_string(OutputIt out, std::span<const std::byte> data)
{
    std::array<char, 2> buf {};

    for (const auto c : data) {
        detail::byte_to_hex(buf.data(), c);
        std::copy_n(buf, 2, out);
    }
    return out;
}

} // namespace bencode