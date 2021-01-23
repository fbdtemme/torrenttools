#pragma once

#include <array>
#include <vector>
#include <algorithm>
#include <numeric>
#include <concepts>
#include <stdexcept>
#include <string_view>
#include <span>

#include <gsl-lite/gsl-lite.hpp>
#include <nonstd/expected.hpp>

#include "dottorrent/utils.hpp"
#include "dottorrent/literals.hpp"
#include "dottorrent/hash_function.hpp"
#include "dottorrent/hex.hpp"

namespace dottorrent {

//============================================================================//
//  dottorrent::basic_hash<N>                                                 //
//============================================================================//

using namespace dottorrent::literals;


template <std::size_t N>
class basic_hash {
public:
    using value_type = std::byte;
    using storage_type = std::array<std::byte, N>;

    using iterator = typename storage_type::iterator;
    using const_iterator = typename storage_type::const_iterator;
    using size_type = std::size_t;

    static constexpr std::size_t size_bits = 8 * N;
    static constexpr std::size_t size_bytes = N;
    static constexpr std::size_t size_hex = 2 * N;

    constexpr basic_hash() noexcept = default;

    explicit constexpr basic_hash(const storage_type& hash) noexcept
        : data_(hash) { }

    explicit constexpr basic_hash(std::string_view digest)
    {
        if (digest.size() != size_bytes) {
            throw std::invalid_argument("invalid digest size");
        }

        std::transform(std::begin(digest), std::end(digest), std::begin(data_),
                [](auto c) { return std::byte(c); });
    }

//    template <typename T>
//    constexpr basic_hash(std::span<const T, size_bytes> digest)
//    {
//        if constexpr (std::is_same_v<T, std::byte>)
//            std::copy(std::begin(digest), std::end(digest), std::begin(data_));
//        else {
//            std::transform(std::begin(digest), std::end(digest), std::begin(data_),
//                    [](auto c) { return std::byte(c); });
//        }
//    }

    template <typename T>
    explicit constexpr basic_hash(std::span<T> digest)
    {
        if (digest.size() != size_bytes) {
            throw std::invalid_argument("invalid digest size");
        }
        if constexpr (std::is_same_v<std::remove_const_t<T>, std::byte>)
            std::copy(std::begin(digest), std::end(digest), std::begin(data_));
        else {
            std::transform(std::begin(digest), std::end(digest), std::begin(data_),
                    [](auto c) { return std::byte(c); });
        }
    }

    explicit constexpr operator std::string_view() const noexcept
    { return {reinterpret_cast<const char*>(data()), size_bytes}; }

    explicit constexpr operator std::span<const std::byte>() const noexcept
    { return std::span<const std::byte>(data(), size_bytes); }

//    explicit constexpr operator std::span<const std::byte, size_bytes>() const
//    { return std::span<const std::byte, N>(data()); }

    static constexpr auto size() noexcept -> size_type
    { return size_bytes; }

    constexpr auto begin() noexcept -> iterator
    { return data_.begin(); }

    constexpr auto begin() const noexcept -> const_iterator
    { return data_.begin(); }

    constexpr auto end() noexcept -> iterator
    { return data_.end(); }

    constexpr auto end() const noexcept -> const_iterator
    { return data_.end(); }

    constexpr auto value() const noexcept -> storage_type&
    { return data_; }

    constexpr auto data() noexcept -> value_type*
    { return data_.data(); }

    constexpr auto data() const noexcept -> const value_type*
    { return data_.data(); }

    auto hex_string() const -> std::string
    {
        std::string s(size_bytes * 2, ' ');
        for (size_t i = 0; i < size_bytes; ++i) {
            auto c = static_cast<const uint8_t>(data_.data()[i]);
            s[2 * i] = detail::int_to_hex_table[(c & 0xF0u)>>4u];
            s[2 * i+1] = detail::int_to_hex_table[c & 0x0Fu];
        }
        return s;
    }

    constexpr bool operator==(const basic_hash& that) const = default;
    constexpr std::strong_ordering operator<=>(const basic_hash& that) const  = default;

private:
    storage_type data_ {std::byte(0)};
};


template <typename T>
concept hash_type = requires {
    {T::size_bytes} -> std::convertible_to<std::size_t>;
    { std::is_base_of_v<basic_hash<T::size_bytes>, T> };
};


} // namespace dottorrent

namespace std {
template <size_t N>
struct hash<dottorrent::basic_hash<N>> {
    std::size_t operator()(const dottorrent::basic_hash<N>& h) const noexcept
    {
        return std::hash<std::string_view>{}(std::string_view(h));
    }
};
}  /* namespace std */


namespace dottorrent {

template<hash_type T>
constexpr auto make_hash_from_hex(std::string_view hex_string)
{
    if (hex_string.size() != T::size_hex)
        throw std::invalid_argument("invalid digest size");

    auto data = from_hexadecimal_string<T::size_bytes>(hex_string);
    if (!data)
        throw std::invalid_argument(
                "invalid digest: contains non-hexadecimal characters");

    return T(*data);
}

} // namespace dottorrent

#define DOTTORRENT_HASH_CLASS_TEMPLATE(NAME, BITS)                          \
namespace dottorrent {                                                      \
struct NAME##_hash : public basic_hash<BITS##_bits>                         \
{                                                                           \
    using base = basic_hash<BITS##_bits>;                                   \
    using base::base;                                                       \
    static constexpr hash_function algorithm = hash_function::NAME;         \
};                                                                          \
}                                                                               \
namespace std {                                                                 \
template <>                                                                     \
struct hash<dottorrent::NAME##_hash> {                                           \
    std::size_t operator()(const dottorrent::NAME##_hash& h) const noexcept   \
    {                                                                           \
        return std::hash<std::string_view>{}(std::string_view(h));              \
    }                                                                           \
};                                                                              \
}  /* namespace std */                                                          \



DOTTORRENT_HASH_CLASS_TEMPLATE(md4,        128)
DOTTORRENT_HASH_CLASS_TEMPLATE(md5,        128)
DOTTORRENT_HASH_CLASS_TEMPLATE(blake2b_512, 512)
DOTTORRENT_HASH_CLASS_TEMPLATE(blake2s_256, 256)
DOTTORRENT_HASH_CLASS_TEMPLATE(sha1,       160)
DOTTORRENT_HASH_CLASS_TEMPLATE(sha224,     224)
DOTTORRENT_HASH_CLASS_TEMPLATE(sha256,     256)
DOTTORRENT_HASH_CLASS_TEMPLATE(sha384,     384)
DOTTORRENT_HASH_CLASS_TEMPLATE(sha512,     512)
DOTTORRENT_HASH_CLASS_TEMPLATE(sha3_224,   224)
DOTTORRENT_HASH_CLASS_TEMPLATE(sha3_256,   256)
DOTTORRENT_HASH_CLASS_TEMPLATE(sha3_384,   284)
DOTTORRENT_HASH_CLASS_TEMPLATE(sha3_512,   512)
DOTTORRENT_HASH_CLASS_TEMPLATE(shake128,   128)
DOTTORRENT_HASH_CLASS_TEMPLATE(shake256,   256)
DOTTORRENT_HASH_CLASS_TEMPLATE(ripemd160,  160)
DOTTORRENT_HASH_CLASS_TEMPLATE(whirlpool,  512)


namespace dottorrent {


using hash_types = std::tuple<
        md4_hash,
        md5_hash,
        blake2b_512_hash,
        blake2s_256_hash,
        sha1_hash,
        sha224_hash,
        sha256_hash,
        sha384_hash,
        sha512_hash,
        sha3_224_hash,
        sha3_256_hash,
        sha3_384_hash,
        sha3_512_hash,
        shake128_hash,
        shake256_hash,
        ripemd160_hash,
        whirlpool_hash>;


//============================================================================//
// Hashers                                                                    //
//============================================================================//
//
//template <typename T>
//concept hasher =
//        std::is_default_constructible_v<T> &&
//        requires (T x) {
//            typename T::hash_type;
//            { x.update(std::span<const std::byte>()) };
//            { x.finalize() } -> std::same_as<typename T::hash_type>;
//        };


} // namespace dottorrent

