#pragma once

#include <span>
#include <string>
#include <compare>
#include <ranges>

#include "dottorrent/hash.hpp"

namespace dottorrent {

namespace rng = std::ranges;

/// Checksum interface.
struct checksum
{
    virtual auto algorithm() const -> hash_function = 0;
    virtual auto name() const -> std::string_view = 0;
    virtual auto size() const -> std::size_t = 0;
    virtual auto value() const -> std::span<const std::byte> = 0;
    virtual auto value() -> std::span<std::byte> = 0;

    virtual auto hex_string() const -> std::string
    {
        return to_hexadecimal_string(value());
    }

    virtual ~checksum() noexcept = default;

    constexpr bool operator==(const checksum& other) const noexcept
    {
        if (this->name() != other.name()) return false;
        return rng::equal(this->value(), other.value());
    }

    constexpr std::partial_ordering operator<=>(const checksum& other) const noexcept
    {
        if (this->name() != other.name()) return std::partial_ordering::unordered;
        return std::lexicographical_compare_three_way(
                std::begin(this->value()), std::end(this->value()),
                std::begin(other.value()), std::end(other.value()));
    }
};


/// @brief Checksum class
class generic_checksum : public checksum
{
public:
    using value_type = std::vector<std::byte>;

    explicit generic_checksum(std::string_view key)
            : data_()
            , key_(key)
    {}

    generic_checksum(std::string_view key, std::span<const std::byte> value)
            : data_(std::begin(value), std::end(value))
            , key_(key)
    { }

    generic_checksum(std::string_view key, std::span<const char> value)
            : data_()
            , key_(key)
    {
        data_.reserve(value.size());
        std::transform(std::begin(value), std::end(value), std::begin(data_),
                [](char i) { return std::byte(i); });
    }

    /// Generic hash functions do not have a known hash_function enum value.
    auto algorithm() const -> hash_function override
    { return static_cast<hash_function>(0); }

    auto name() const -> std::string_view override
    { return key_; }

    auto size() const -> std::size_t override
    { return data_.size(); }

    auto value() const -> std::span<const std::byte> override
    { return std::span<const std::byte>(data_); }

    auto value() -> std::span<std::byte> override
    { return std::span<std::byte>(data_); }

private:
    value_type data_;
    std::string key_;
};


template <hash_type T>
class basic_checksum : public checksum
{
public:
    using storage_type = T;

    basic_checksum() = default;

    template <typename... Args>
        requires (sizeof...(Args) > 0)
    explicit basic_checksum(Args&&... args)
            : data_(std::forward<Args>(args)...)
    { };

    explicit basic_checksum(storage_type&& s)
            : data_(std::move(s))
    { };

    auto algorithm() const -> hash_function override
    { return storage_type::algorithm; }

    auto name() const -> std::string_view override
    { return to_string(storage_type::algorithm); }

    auto size() const -> std::size_t override
    { return data_.size(); }

    auto value() const -> std::span<const std::byte> override
    { return std::span<const std::byte>(data_); }

    auto value() -> std::span<std::byte> override
    { return std::span<std::byte>(data_); }

private:
    storage_type data_;
};


#define DOTTORRENT_CHECKSUM_CLASS_TEMPLATE(NAME)                            \
using NAME##_checksum = basic_checksum<NAME##_hash>;                        \

DOTTORRENT_CHECKSUM_CLASS_TEMPLATE(md4);
DOTTORRENT_CHECKSUM_CLASS_TEMPLATE(md5);
DOTTORRENT_CHECKSUM_CLASS_TEMPLATE(blake2b_512);
DOTTORRENT_CHECKSUM_CLASS_TEMPLATE(blake2s_256);
DOTTORRENT_CHECKSUM_CLASS_TEMPLATE(sha1);
DOTTORRENT_CHECKSUM_CLASS_TEMPLATE(sha224);
DOTTORRENT_CHECKSUM_CLASS_TEMPLATE(sha256);
DOTTORRENT_CHECKSUM_CLASS_TEMPLATE(sha384);
DOTTORRENT_CHECKSUM_CLASS_TEMPLATE(sha512);
DOTTORRENT_CHECKSUM_CLASS_TEMPLATE(sha3_224);
DOTTORRENT_CHECKSUM_CLASS_TEMPLATE(sha3_256);
DOTTORRENT_CHECKSUM_CLASS_TEMPLATE(sha3_384);
DOTTORRENT_CHECKSUM_CLASS_TEMPLATE(sha3_512);
DOTTORRENT_CHECKSUM_CLASS_TEMPLATE(shake128);
DOTTORRENT_CHECKSUM_CLASS_TEMPLATE(shake256);
DOTTORRENT_CHECKSUM_CLASS_TEMPLATE(ripemd160);
DOTTORRENT_CHECKSUM_CLASS_TEMPLATE(whirlpool);


using checksum_types = std::tuple<
        md4_checksum,
        md5_checksum,
        blake2b_512_checksum,
        blake2s_256_checksum,
        sha1_checksum,
        sha224_checksum,
        sha256_checksum,
        sha384_checksum,
        sha512_checksum,
        sha3_224_checksum,
        sha3_256_checksum,
        sha3_384_checksum,
        sha3_512_checksum,
        shake128_checksum,
        shake256_checksum,
        ripemd160_checksum,
        whirlpool_checksum>;

#define DOTTORRENT_MAKE_CHECKSUM_KEY_TEMPLATE(KEY, NAME, ...)   \
    if (key == to_string(hash_function::NAME)) {                \
        return std::make_unique<NAME##_checksum>(__VA_ARGS__);  \
    }                                                               \

#define DOTTORRENT_MAKE_CHECKSUM_FN_TEMPLATE(FN, NAME, ...)     \
    if (FN == hash_function::NAME) {                            \
        return std::make_unique<NAME##_checksum>(__VA_ARGS__);  \
    }                                                           \




/// Create a checksum from a runtime string key
inline auto make_checksum(std::string_view key, std::span<const std::byte> value) -> std::unique_ptr<checksum>
{
    DOTTORRENT_MAKE_CHECKSUM_KEY_TEMPLATE(key, md4,        value)
    DOTTORRENT_MAKE_CHECKSUM_KEY_TEMPLATE(key, md5,        value)
    DOTTORRENT_MAKE_CHECKSUM_KEY_TEMPLATE(key, blake2b_512, value)
    DOTTORRENT_MAKE_CHECKSUM_KEY_TEMPLATE(key, blake2s_256, value)
    DOTTORRENT_MAKE_CHECKSUM_KEY_TEMPLATE(key, sha1,       value)
    DOTTORRENT_MAKE_CHECKSUM_KEY_TEMPLATE(key, sha224,     value)
    DOTTORRENT_MAKE_CHECKSUM_KEY_TEMPLATE(key, sha256,     value)
    DOTTORRENT_MAKE_CHECKSUM_KEY_TEMPLATE(key, sha384,     value)
    DOTTORRENT_MAKE_CHECKSUM_KEY_TEMPLATE(key, sha512,     value)
    DOTTORRENT_MAKE_CHECKSUM_KEY_TEMPLATE(key, sha3_224,   value)
    DOTTORRENT_MAKE_CHECKSUM_KEY_TEMPLATE(key, sha3_256,   value)
    DOTTORRENT_MAKE_CHECKSUM_KEY_TEMPLATE(key, sha3_384,   value)
    DOTTORRENT_MAKE_CHECKSUM_KEY_TEMPLATE(key, sha3_512,   value)
    DOTTORRENT_MAKE_CHECKSUM_KEY_TEMPLATE(key, shake128,   value)
    DOTTORRENT_MAKE_CHECKSUM_KEY_TEMPLATE(key, shake256,   value)
    DOTTORRENT_MAKE_CHECKSUM_KEY_TEMPLATE(key, ripemd160,  value)
    DOTTORRENT_MAKE_CHECKSUM_KEY_TEMPLATE(key, whirlpool,  value)

    return std::make_unique<generic_checksum>(key, value);
}

/// Create a checksum from a runtime string key
inline auto make_checksum(std::string_view key) -> std::unique_ptr<checksum>
{
    DOTTORRENT_MAKE_CHECKSUM_KEY_TEMPLATE(key, md4)
    DOTTORRENT_MAKE_CHECKSUM_KEY_TEMPLATE(key, md5)
    DOTTORRENT_MAKE_CHECKSUM_KEY_TEMPLATE(key, blake2b_512)
    DOTTORRENT_MAKE_CHECKSUM_KEY_TEMPLATE(key, blake2s_256)
    DOTTORRENT_MAKE_CHECKSUM_KEY_TEMPLATE(key, sha1)
    DOTTORRENT_MAKE_CHECKSUM_KEY_TEMPLATE(key, sha224)
    DOTTORRENT_MAKE_CHECKSUM_KEY_TEMPLATE(key, sha256)
    DOTTORRENT_MAKE_CHECKSUM_KEY_TEMPLATE(key, sha384)
    DOTTORRENT_MAKE_CHECKSUM_KEY_TEMPLATE(key, sha512)
    DOTTORRENT_MAKE_CHECKSUM_KEY_TEMPLATE(key, sha3_224)
    DOTTORRENT_MAKE_CHECKSUM_KEY_TEMPLATE(key, sha3_256)
    DOTTORRENT_MAKE_CHECKSUM_KEY_TEMPLATE(key, sha3_384)
    DOTTORRENT_MAKE_CHECKSUM_KEY_TEMPLATE(key, sha3_512)
    DOTTORRENT_MAKE_CHECKSUM_KEY_TEMPLATE(key, shake128)
    DOTTORRENT_MAKE_CHECKSUM_KEY_TEMPLATE(key, shake256)
    DOTTORRENT_MAKE_CHECKSUM_KEY_TEMPLATE(key, ripemd160)
    DOTTORRENT_MAKE_CHECKSUM_KEY_TEMPLATE(key, whirlpool)

    return std::make_unique<generic_checksum>(key);
}

#undef DOTTORRENT_MAKE_CHECKSUM_KEY_TEMPLATE

inline auto make_checksum(hash_function f, std::span<const std::byte> value) -> std::unique_ptr<checksum>
{
    DOTTORRENT_MAKE_CHECKSUM_FN_TEMPLATE(f, md4,        value)
    DOTTORRENT_MAKE_CHECKSUM_FN_TEMPLATE(f, md5,        value)
    DOTTORRENT_MAKE_CHECKSUM_FN_TEMPLATE(f, blake2b_512, value)
    DOTTORRENT_MAKE_CHECKSUM_FN_TEMPLATE(f, blake2s_256, value)
    DOTTORRENT_MAKE_CHECKSUM_FN_TEMPLATE(f, sha1,       value)
    DOTTORRENT_MAKE_CHECKSUM_FN_TEMPLATE(f, sha224,     value)
    DOTTORRENT_MAKE_CHECKSUM_FN_TEMPLATE(f, sha256,     value)
    DOTTORRENT_MAKE_CHECKSUM_FN_TEMPLATE(f, sha384,     value)
    DOTTORRENT_MAKE_CHECKSUM_FN_TEMPLATE(f, sha512,     value)
    DOTTORRENT_MAKE_CHECKSUM_FN_TEMPLATE(f, sha3_224,   value)
    DOTTORRENT_MAKE_CHECKSUM_FN_TEMPLATE(f, sha3_256,   value)
    DOTTORRENT_MAKE_CHECKSUM_FN_TEMPLATE(f, sha3_384,   value)
    DOTTORRENT_MAKE_CHECKSUM_FN_TEMPLATE(f, sha3_512,   value)
    DOTTORRENT_MAKE_CHECKSUM_FN_TEMPLATE(f, shake128,   value)
    DOTTORRENT_MAKE_CHECKSUM_FN_TEMPLATE(f, shake256,   value)
    DOTTORRENT_MAKE_CHECKSUM_FN_TEMPLATE(f, ripemd160,  value)
    DOTTORRENT_MAKE_CHECKSUM_FN_TEMPLATE(f, whirlpool,  value)

    throw std::invalid_argument("invalid hash function");
}

inline auto make_checksum(hash_function f) -> std::unique_ptr<checksum>
{
    DOTTORRENT_MAKE_CHECKSUM_FN_TEMPLATE(f, md4)
    DOTTORRENT_MAKE_CHECKSUM_FN_TEMPLATE(f, md5)
    DOTTORRENT_MAKE_CHECKSUM_FN_TEMPLATE(f, blake2b_512)
    DOTTORRENT_MAKE_CHECKSUM_FN_TEMPLATE(f, blake2s_256)
    DOTTORRENT_MAKE_CHECKSUM_FN_TEMPLATE(f, sha1)
    DOTTORRENT_MAKE_CHECKSUM_FN_TEMPLATE(f, sha224)
    DOTTORRENT_MAKE_CHECKSUM_FN_TEMPLATE(f, sha256)
    DOTTORRENT_MAKE_CHECKSUM_FN_TEMPLATE(f, sha384)
    DOTTORRENT_MAKE_CHECKSUM_FN_TEMPLATE(f, sha512)
    DOTTORRENT_MAKE_CHECKSUM_FN_TEMPLATE(f, sha3_224)
    DOTTORRENT_MAKE_CHECKSUM_FN_TEMPLATE(f, sha3_256)
    DOTTORRENT_MAKE_CHECKSUM_FN_TEMPLATE(f, sha3_384)
    DOTTORRENT_MAKE_CHECKSUM_FN_TEMPLATE(f, sha3_512)
    DOTTORRENT_MAKE_CHECKSUM_FN_TEMPLATE(f, shake128)
    DOTTORRENT_MAKE_CHECKSUM_FN_TEMPLATE(f, shake256)
    DOTTORRENT_MAKE_CHECKSUM_FN_TEMPLATE(f, ripemd160)
    DOTTORRENT_MAKE_CHECKSUM_FN_TEMPLATE(f, whirlpool)

    throw std::invalid_argument("invalid hash function");
}


inline std::unique_ptr<checksum> make_checksum(std::string_view key, std::string_view value)
{
    return make_checksum(key, {reinterpret_cast<const std::byte*>(value.data()), value.size()});
}

inline std::unique_ptr<checksum> make_checksum(hash_function f, std::string_view value)
{
    return make_checksum(f, {reinterpret_cast<const std::byte*>(value.data()), value.size()});
}

inline std::unique_ptr<checksum> make_checksum_from_hex(std::string_view key, std::string_view hex_string)
{
    std::vector<std::byte> data {};
    from_hexadecimal_string(std::back_inserter(data), hex_string);
    if (data.size() != hex_string.size() / 2)
        throw std::invalid_argument(
                "invalid digest: contains non-hexadecimal characters");

    return make_checksum(key, std::move(data));
}


inline std::unique_ptr<checksum> make_checksum_from_hex(hash_function key, std::string_view hex_string)
{
    std::vector<std::byte> data {};
    from_hexadecimal_string(std::back_inserter(data), hex_string);
    if (data.size() != hex_string.size() / 2)
        throw std::invalid_argument("invalid digest: contains non-hexadecimal characters");

    return make_checksum(key, std::move(data));
}

} // namespace dottorrent