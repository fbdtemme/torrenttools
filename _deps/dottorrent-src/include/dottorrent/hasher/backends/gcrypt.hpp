#pragma once

#include <exception>
#include <system_error>
#include <type_traits>
#include <span>
#include <gsl-lite/gsl-lite.hpp>
#include <cstring>

#include <gcrypt.h>

namespace gcrypt {
namespace detail {

class gcrypt_category : public std::error_category
{
public:
    gcrypt_category() = default;

    const char* name() const noexcept override
    {
        return "gcrypt";
    }

    std::string message(int value) const override
    {
        const char* src = ::gcry_strsource((::gcry_error_t) value);
        const char* err = ::gcry_strerror((::gcry_error_t) value);

        return (src && err)
               ? std::string(src)+' '+err
               : "gcrypt error";
    }
};

} // namespace detail

const std::error_category& gcrypt_category();

inline std::error_code make_error_code(gcry_error_t errc)
{
    return std::error_code(static_cast<int>(errc), gcrypt_category());
}

} // namespace gcrypt

namespace std {

template <>
class is_error_code_enum<::gcry_error_t> : std::true_type { };

}

namespace gcrypt {

enum class message_digest_algorithm
{
    none          = GCRY_MD_NONE,                ///< This is not a real algorithm but used by some functions as an error return value. This constant is guaranteed to have the value 0.
    sha1          = GCRY_MD_SHA1,                ///< This is the SHA-1 algorithm which yields a message digest of 20 bytes. Note that SHA-1 begins to show some weaknesses and it is suggested to fade out its use if strong cryptographic properties are required.
    rmd160        = GCRY_MD_RMD160,              ///< This is the 160 bit version of the RIPE message digest (RIPE-MD-160). Like SHA-1 it also yields a digest of 20 bytes. This algorithm share a lot of design properties with SHA-1 and thus it is advisable not to use it for new protocols.
    md5           = GCRY_MD_MD5,                 ///< This is the well known MD5 algorithm, which yields a message digest of 16 bytes. Note that the MD5 algorithm has severe weaknesses, for example it is easy to compute two messages yielding the same hash (collision attack). The use of this algorithm is only justified for non-cryptographic application.
    md4           = GCRY_MD_MD4,                 ///< This is the MD4 algorithm, which yields a message digest of 16 bytes. This algorithm has severe weaknesses and should not be used.
    md2           = GCRY_MD_MD2,                 ///< This is an reserved identifier for MD-2; there is no implementation yet. This algorithm has severe weaknesses and should not be used.
    tiger         = GCRY_MD_TIGER,               ///< This is the TIGER/192 algorithm which yields a message digest of 24 bytes. Actually this is a variant of TIGER with a different output print order as used by GnuPG up to version 1.3.2.
    tiger1        = GCRY_MD_TIGER1,              ///< This is the TIGER variant as used by the NESSIE project. It uses the most commonly used output print order.
    tiger2        = GCRY_MD_TIGER2,              ///< This is another variant of TIGER with a different padding scheme.
    haval         = GCRY_MD_HAVAL,               ///< This is an reserved value for the HAVAL algorithm with 5 passes and 160 bit. It yields a message digest of 20 bytes. Note that there is no implementation yet available.
    sha224        = GCRY_MD_SHA224,              ///< This is the SHA-224 algorithm which yields a message digest of 28 bytes. See Change Notice 1 for FIPS 180-2 for the specification.
    sha256        = GCRY_MD_SHA256,              ///< This is the SHA-256 algorithm which yields a message digest of 32 bytes. See FIPS 180-2 for the specification.
    sha384        = GCRY_MD_SHA384,              ///< This is the SHA-384 algorithm which yields a message digest of 48 bytes. See FIPS 180-2 for the specification.
    sha512        = GCRY_MD_SHA512,              ///< This is the SHA-384 algorithm which yields a message digest of 64 bytes. See FIPS 180-2 for the specification.
    sha3_224      = GCRY_MD_SHA3_224,            ///< This is the SHA3-224 algorithm which yields a message digest of 28 bytes. See FIPS 202 for the specification.
    sha3_256      = GCRY_MD_SHA3_256,            ///< This is the SHA3-256 algorithm which yields a message digest of 32 bytes. See FIPS 202 for the specification.
    sha3_384      = GCRY_MD_SHA3_384,            ///< This is the SHA3-384 algorithm which yields a message digest of 48 bytes. See FIPS 202 for the specification.
    sha3_512      = GCRY_MD_SHA3_512,            ///< This is the SHA3-384 algorithm which yields a message digest of 64 bytes. See FIPS 202 for the specification.
    shake128      = GCRY_MD_SHAKE128,            ///< This is the SHAKE128 extendable-output function (XOF) algorithm with 128 bit security strength. See FIPS 202 for the specification.
    shake256      = GCRY_MD_SHAKE256,            ///< This is the SHAKE256 extendable-output function (XOF) algorithm with 256 bit security strength. See FIPS 202 for the specification.
    crc32         = GCRY_MD_CRC32,               ///< This is the ISO 3309 and ITU-T V.42 cyclic redundancy check. It yields an output of 4 bytes. Note that this is not a hash algorithm in the cryptographic sense.
    crc32_rfc1510 = GCRY_MD_CRC32_RFC1510,       ///< This is the above cyclic redundancy check function, as modified by RFC 1510. It yields an output of 4 bytes. Note that this is not a hash algorithm in the cryptographic sense.
    crc24_rfc2440 = GCRY_MD_CRC24_RFC2440,       ///< This is the OpenPGP cyclic redundancy check function. It yields an output of 3 bytes. Note that this is not a hash algorithm in the cryptographic sense.
    whirlpool     = GCRY_MD_WHIRLPOOL,           ///< This is the Whirlpool algorithm which yields a message digest of 64 bytes.
    gostr3411_94  = GCRY_MD_GOSTR3411_94,        ///< This is the hash algorithm described in GOST R 34.11-94 which yields a message digest of 32 bytes.
    stribog256    = GCRY_MD_STRIBOG256,          ///< This is the 256-bit version of hash algorithm described in GOST R 34.11-2012 which yields a message digest of 32 bytes.
    stribog512    = GCRY_MD_STRIBOG512,          ///< This is the 512-bit version of hash algorithm described in GOST R 34.11-2012 which yields a message digest of 64 bytes.
    blake2b_512   = GCRY_MD_BLAKE2B_512,         ///< This is the BLAKE2b-512 algorithm which yields a message digest of 64 bytes. See RFC 7693 for the specification.
    blake2b_384   = GCRY_MD_BLAKE2B_384,         ///< This is the BLAKE2b-384 algorithm which yields a message digest of 48 bytes. See RFC 7693 for the specification.
    blake2b_256   = GCRY_MD_BLAKE2B_256,         ///< This is the BLAKE2b-256 algorithm which yields a message digest of 32 bytes. See RFC 7693 for the specification.
    blake2b_160   = GCRY_MD_BLAKE2B_160,         ///< This is the BLAKE2b-160 algorithm which yields a message digest of 20 bytes. See RFC 7693 for the specification.
    blake2s_256   = GCRY_MD_BLAKE2S_256,         ///< This is the BLAKE2s-256 algorithm which yields a message digest of 32 bytes. See RFC 7693 for the specification.
    blake2s_224   = GCRY_MD_BLAKE2S_224,         ///< This is the BLAKE2s-224 algorithm which yields a message digest of 28 bytes. See RFC 7693 for the specification.
    blake2s_160   = GCRY_MD_BLAKE2S_160,         ///< This is the BLAKE2s-160 algorithm which yields a message digest of 20 bytes. See RFC 7693 for the specification.
    blake2s_128   = GCRY_MD_BLAKE2S_128,         ///< This is the BLAKE2s-128 algorithm which yields a message digest of 16 bytes. See RFC 7693 for the specification.
};


class message_digest
{
public:
    explicit message_digest(message_digest_algorithm algorithm, gcry_md_flags flags = gcry_md_flags(0))
            : context_()
            , algorithm_(static_cast<gcry_md_algos>(algorithm))
    {
        gcry_error_t err = ::gcry_md_open(&context_, algorithm_, flags);
        if (err != 0) {
            throw std::system_error((int)err, gcrypt_category());
        }
    }

    void update(std::span<const std::byte> data)
    {
        ::gcry_md_write(context_, data.data(), data.size());
    }

    void finalize_to(std::span<std::byte> digest)
    {
        const unsigned char* data = ::gcry_md_read(context_, algorithm_);
        const auto digest_size = ::gcry_md_get_algo_dlen(algorithm_);

        Expects(digest.size() >= digest_size);
        std::memcpy(digest.data(), data, digest_size);
    }

    void reset()
    {
        ::gcry_md_reset(context_);
    }

    ~message_digest()
    {
        ::gcry_md_close(context_);
    }

private:
    ::gcry_md_hd_t context_;
    ::gcry_md_algos algorithm_;
};

} // namespace gcrypt
