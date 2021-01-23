#pragma once

#include "dottorrent/hash_function.hpp"
#include "dottorrent/hasher/hasher.hpp"
#include "dottorrent/hasher/backends/openssl.hpp"

namespace dottorrent {

namespace detail {

static const auto supported_hash_functions_openssl = std::unordered_set {
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

constexpr openssl::message_digest_algorithm get_openssl_algorithm(hash_function function)
{
    using namespace openssl;

    switch (function) {
    case hash_function::md4:          return message_digest_algorithm::md4;
    case hash_function::md5:          return message_digest_algorithm::md5;
    case hash_function::blake2b_512:  return message_digest_algorithm::blake2b512;
    case hash_function::blake2s_256:  return message_digest_algorithm::blake2s256;
    case hash_function::sha1:         return message_digest_algorithm::sha1;
    case hash_function::sha224:       return message_digest_algorithm::sha224;
    case hash_function::sha256:       return message_digest_algorithm::sha256;
    case hash_function::sha384:       return message_digest_algorithm::sha384;
    case hash_function::sha512:       return message_digest_algorithm::sha512;
    case hash_function::sha3_224:     return message_digest_algorithm::sha3_224;
    case hash_function::sha3_256:     return message_digest_algorithm::sha3_256;
    case hash_function::sha3_384:     return message_digest_algorithm::sha3_384;
    case hash_function::sha3_512:     return message_digest_algorithm::sha3_512;
    case hash_function::shake128:     return message_digest_algorithm::shake128;
    case hash_function::shake256:     return message_digest_algorithm::shake256;
    case hash_function::ripemd160:    return message_digest_algorithm::ripemd160;
    case hash_function::whirlpool:    return message_digest_algorithm::whirlpool;

    default:
        throw std::invalid_argument("No matching openssl algorithm.");
    }
}
}

class openssl_hasher : public hasher
{
public:
    explicit openssl_hasher(hash_function algorithm)
            : digest_(detail::get_openssl_algorithm(algorithm))
    {}

    void update(std::span<const std::byte> data) override
    {
        digest_.update(data);
    }

    void finalize_to(std::span<std::byte> out) override
    {
        digest_.finalize_to(out);
        digest_.reset();
    }

    static const std::unordered_set<hash_function>& supported_algorithms() noexcept
    {
        return detail::supported_hash_functions_openssl;
    }

private:
    openssl::message_digest digest_;
};

}