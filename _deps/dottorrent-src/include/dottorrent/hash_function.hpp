#pragma once
#include <cstdint>
#include <string_view>
#include <optional>
#include <stdexcept>
#include <algorithm>

namespace dottorrent {

using namespace std::string_view_literals;

enum class hash_function {
    none,
    md4,
    md5,
    blake2b_512,
    blake2s_256,
    sha1,
    sha224,
    sha256,
    sha384,
    sha512,
    sha3_224,
    sha3_256,
    sha3_384,
    sha3_512,
    shake128,
    shake256,
    ripemd160,
    whirlpool,
};

/// List of supported algorithms to calculate checksums.
constexpr std::array supported_hash_functions {
        hash_function::md4,
        hash_function::md5,
        hash_function::blake2b_512,
        hash_function::blake2s_256,
        hash_function::sha1,
        hash_function::sha224,
        hash_function::sha256,
        hash_function::sha384,
        hash_function::sha512,
        hash_function::sha3_224,
        hash_function::sha3_256,
        hash_function::sha3_384,
        hash_function::sha3_512,
        hash_function::shake128,
        hash_function::shake256,
        hash_function::ripemd160,
        hash_function::whirlpool,
};

constexpr std::string_view to_string(hash_function f)
{
    switch (f) {
        case hash_function::md4:         return "md4";
        case hash_function::md5:         return "md5";
        case hash_function::blake2b_512: return "blake2b_512";
        case hash_function::blake2s_256: return "blake2s_256";
        case hash_function::sha1:        return "sha1";
        case hash_function::sha224:      return "sha224";
        case hash_function::sha256:      return "sha256";
        case hash_function::sha384:      return "sha384";
        case hash_function::sha512:      return "sha512";
        case hash_function::sha3_224:    return "sha3_224";
        case hash_function::sha3_256:    return "sha3_256";
        case hash_function::sha3_384:    return "sha3_384";
        case hash_function::sha3_512:    return "sha3_512";
        case hash_function::shake128:    return "shake128";
        case hash_function::shake256:    return "shake256";
        case hash_function::ripemd160:   return "ripemd160";
        case hash_function::whirlpool:   return "whirlpool";
    default:
        throw std::invalid_argument("invalid hash_function");
    }
}

constexpr bool is_hash_function_name(std::string_view s)
{
    return std::apply([=](auto... v){
        return ((to_string(v) == s) || ... );
    }, supported_hash_functions);
}

constexpr auto make_hash_function(std::string_view s) -> std::optional<hash_function>
{
    for (auto func: supported_hash_functions) {
        if (s == to_string(func)) return func;
    }
    return std::nullopt;
}

}