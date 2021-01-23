#pragma once

#include "dottorrent/hash_function.hpp"
#include "dottorrent/hash.hpp"
#include "dottorrent/checksum.hpp"


namespace dottorrent {

template <hash_function F>
struct hash_function_traits {};

#define DOTTORRENT_REGISTER_HASH_TRAIT(name)        \
template <>                                         \
struct hash_function_traits<hash_function::name>    \
{                                                   \
    using hash_type     = name##_hash;              \
    using checksum_type = name##_checksum;          \
    static constexpr std::size_t digest_size_bytes  = hash_type::size_bytes ;    \
    static constexpr std::size_t digest_size_bits   = hash_type::size_bits ;     \
    static constexpr std::string_view name  = to_string(hash_function::name);  \
};                                                  \


DOTTORRENT_REGISTER_HASH_TRAIT(md4)
DOTTORRENT_REGISTER_HASH_TRAIT(md5)
DOTTORRENT_REGISTER_HASH_TRAIT(blake2b_512)
DOTTORRENT_REGISTER_HASH_TRAIT(blake2s_256)
DOTTORRENT_REGISTER_HASH_TRAIT(sha1)
DOTTORRENT_REGISTER_HASH_TRAIT(sha224)
DOTTORRENT_REGISTER_HASH_TRAIT(sha256)
DOTTORRENT_REGISTER_HASH_TRAIT(sha384)
DOTTORRENT_REGISTER_HASH_TRAIT(sha512)
DOTTORRENT_REGISTER_HASH_TRAIT(sha3_224)
DOTTORRENT_REGISTER_HASH_TRAIT(sha3_256)
DOTTORRENT_REGISTER_HASH_TRAIT(sha3_384)
DOTTORRENT_REGISTER_HASH_TRAIT(sha3_512)
DOTTORRENT_REGISTER_HASH_TRAIT(shake128)
DOTTORRENT_REGISTER_HASH_TRAIT(shake256)
DOTTORRENT_REGISTER_HASH_TRAIT(ripemd160)
DOTTORRENT_REGISTER_HASH_TRAIT(whirlpool)

#undef DOTTORRENT_REGISTER_HASH_TRAIT

} // namespace dottorrent