#pragma once

#include <type_traits>
#include <system_error>
#include <span>


#include <openssl/crypto.h>
#include <openssl/evp.h>
#include <openssl/err.h>


namespace openssl {

namespace detail {

class openssl_crypto_category : public std::error_category
{
public:
    openssl_crypto_category() = default;

    const char* name() const noexcept override
    {
        return "openssl-crypto";
    }

    std::string message(int value) const override
    {
        const char* s = ::ERR_reason_error_string(value);
        return s ? s : "openssl-crypto error";
    }
};

}

const std::error_category& openssl_crypto_category();

enum class evp_errc
{
#ifdef EVP_R_AES_KEY_SETUP_FAILED
aes_key_setup_failed                            = EVP_R_AES_KEY_SETUP_FAILED,
#endif
#ifdef EVP_R_ARIA_KEY_SETUP_FAILED
aria_key_setup_failed                           = EVP_R_ARIA_KEY_SETUP_FAILED,
#endif
#ifdef EVP_R_BAD_DECRYPT
bad_decrypt                                     = EVP_R_BAD_DECRYPT,
#endif
#ifdef EVP_R_BAD_KEY_LENGTH
bad_key_length                                  = EVP_R_BAD_KEY_LENGTH,
#endif
#ifdef EVP_R_BUFFER_TOO_SMALL
buffer_too_small                                = EVP_R_BUFFER_TOO_SMALL,
#endif
#ifdef EVP_R_CAMELLIA_KEY_SETUP_FAILED
camellia_key_setup_failed                       = EVP_R_CAMELLIA_KEY_SETUP_FAILED,
#endif
#ifdef EVP_R_CIPHER_PARAMETER_ERROR
cipher_parameter_error                          = EVP_R_CIPHER_PARAMETER_ERROR,
#endif
#ifdef EVP_R_COMMAND_NOT_SUPPORTED
command_not_supported                           = EVP_R_COMMAND_NOT_SUPPORTED,
#endif
#ifdef EVP_R_COPY_ERROR
copy_error                                      = EVP_R_COPY_ERROR,
#endif
#ifdef EVP_R_CTRL_NOT_IMPLEMENTED
ctrl_not_implemented                            = EVP_R_CTRL_NOT_IMPLEMENTED,
#endif
#ifdef EVP_R_CTRL_OPERATION_NOT_IMPLEMENTED
ctrl_operation_not_implemented                  = EVP_R_CTRL_OPERATION_NOT_IMPLEMENTED,
#endif
#ifdef EVP_R_DATA_NOT_MULTIPLE_OF_BLOCK_LENGTH
data_not_multiple_of_block_length               = EVP_R_DATA_NOT_MULTIPLE_OF_BLOCK_LENGTH,
#endif
#ifdef EVP_R_DECODE_ERROR
decode_error                                    = EVP_R_DECODE_ERROR,
#endif
#ifdef EVP_R_DISABLED_FOR_FIPS
disabled_for_fips                               = EVP_R_DISABLED_FOR_FIPS,
#endif
#ifdef EVP_R_DIFFERENT_KEY_TYPES
different_key_types                             = EVP_R_DIFFERENT_KEY_TYPES,
#endif
#ifdef EVP_R_DIFFERENT_PARAMETERS
different_parameters                            = EVP_R_DIFFERENT_PARAMETERS,
#endif
#ifdef EVP_R_ERROR_LOADING_SECTION
error_loading_section                           = EVP_R_ERROR_LOADING_SECTION,
#endif
#ifdef EVP_R_ERROR_SETTING_FIPS_MODE
error_setting_fips_mode                         = EVP_R_ERROR_SETTING_FIPS_MODE,
#endif
#ifdef EVP_R_EXPECTING_AN_HMAC_KEY
expecting_an_hmac_key                           = EVP_R_EXPECTING_AN_HMAC_KEY,
#endif
#ifdef EVP_R_EXPECTING_AN_RSA_KEY
expecting_an_rsa_key                            = EVP_R_EXPECTING_AN_RSA_KEY,
#endif
#ifdef EVP_R_EXPECTING_A_DH_KEY
expecting_a_dh_key                              = EVP_R_EXPECTING_A_DH_KEY,
#endif
#ifdef EVP_R_EXPECTING_A_DSA_KEY
expecting_a_dsa_key                             = EVP_R_EXPECTING_A_DSA_KEY,
#endif
#ifdef EVP_R_EXPECTING_A_EC_KEY
expecting_a_ec_key                              = EVP_R_EXPECTING_A_EC_KEY,
#endif
#ifdef EVP_R_EXPECTING_A_POLY1305_KEY
expecting_a_poly1305_key                        = EVP_R_EXPECTING_A_POLY1305_KEY,
#endif
#ifdef EVP_R_EXPECTING_A_SIPHASH_KEY
expecting_a_siphash_key                         = EVP_R_EXPECTING_A_SIPHASH_KEY,
#endif
#ifdef EVP_R_FIPS_MODE_NOT_SUPPORTED
fips_mode_not_supported                         = EVP_R_FIPS_MODE_NOT_SUPPORTED,
#endif
#ifdef EVP_R_GET_RAW_KEY_FAILED
get_raw_key_failed                              = EVP_R_GET_RAW_KEY_FAILED,
#endif
#ifdef EVP_R_ILLEGAL_SCRYPT_PARAMETERS
illegal_scrypt_parameters                       = EVP_R_ILLEGAL_SCRYPT_PARAMETERS,
#endif
#ifdef EVP_R_INITIALIZATION_ERROR
initialization_error                            = EVP_R_INITIALIZATION_ERROR,
#endif
#ifdef EVP_R_INPUT_NOT_INITIALIZED
input_not_initialized                           = EVP_R_INPUT_NOT_INITIALIZED,
#endif
#ifdef EVP_R_INVALID_DIGEST
invalid_digest                                  = EVP_R_INVALID_DIGEST,
#endif
#ifdef EVP_R_INVALID_FIPS_MODE
invalid_fips_mode                               = EVP_R_INVALID_FIPS_MODE,
#endif
#ifdef EVP_R_INVALID_IV_LENGTH
invalid_iv_length                               = EVP_R_INVALID_IV_LENGTH,
#endif
#ifdef EVP_R_INVALID_KEY
invalid_key                                     = EVP_R_INVALID_KEY,
#endif
#ifdef EVP_R_INVALID_KEY_LENGTH
invalid_key_length                              = EVP_R_INVALID_KEY_LENGTH,
#endif
#ifdef EVP_R_INVALID_OPERATION
invalid_operation                               = EVP_R_INVALID_OPERATION,
#endif
#ifdef EVP_R_KEYGEN_FAILURE
keygen_failure                                  = EVP_R_KEYGEN_FAILURE,
#endif
#ifdef EVP_R_KEY_SETUP_FAILED
key_setup_failed                                = EVP_R_KEY_SETUP_FAILED,
#endif
#ifdef EVP_R_MEMORY_LIMIT_EXCEEDED
memory_limit_exceeded                           = EVP_R_MEMORY_LIMIT_EXCEEDED,
#endif
#ifdef EVP_R_MESSAGE_DIGEST_IS_NULL
message_digest_is_null                          = EVP_R_MESSAGE_DIGEST_IS_NULL,
#endif
#ifdef EVP_R_METHOD_NOT_SUPPORTED
method_not_supported                            = EVP_R_METHOD_NOT_SUPPORTED,
#endif
#ifdef EVP_R_MISSING_PARAMETERS
missing_parameters                              = EVP_R_MISSING_PARAMETERS,
#endif
#ifdef EVP_R_NOT_XOF_OR_INVALID_LENGTH
not_xof_or_invalid_length                       = EVP_R_NOT_XOF_OR_INVALID_LENGTH,
#endif
#ifdef EVP_R_NO_CIPHER_SET
no_cipher_set                                   = EVP_R_NO_CIPHER_SET,
#endif
#ifdef EVP_R_NO_DEFAULT_DIGEST
no_default_digest                               = EVP_R_NO_DEFAULT_DIGEST,
#endif
#ifdef EVP_R_NO_DIGEST_SET
no_digest_set                                   = EVP_R_NO_DIGEST_SET,
#endif
#ifdef EVP_R_NO_KEY_SET
no_key_set                                      = EVP_R_NO_KEY_SET,
#endif
#ifdef EVP_R_NO_OPERATION_SET
no_operation_set                                = EVP_R_NO_OPERATION_SET,
#endif
#ifdef EVP_R_ONLY_ONESHOT_SUPPORTED
only_oneshot_supported                          = EVP_R_ONLY_ONESHOT_SUPPORTED,
#endif
#ifdef EVP_R_OPERATION_NOT_SUPPORTED_FOR_THIS_KEYTYPE
operation_not_supported_for_this_keytype        = EVP_R_OPERATION_NOT_SUPPORTED_FOR_THIS_KEYTYPE,
#endif
#ifdef EVP_R_OPERATON_NOT_INITIALIZED
operaton_not_initialized                        = EVP_R_OPERATON_NOT_INITIALIZED,
#endif
#ifdef EVP_R_PARAMETER_TOO_LARGE
parameter_too_large                             = EVP_R_PARAMETER_TOO_LARGE,
#endif
#ifdef EVP_R_PARTIALLY_OVERLAPPING
partially_overlapping                           = EVP_R_PARTIALLY_OVERLAPPING,
#endif
#ifdef EVP_R_PBKDF2_ERROR
pbkdf2_error                                    = EVP_R_PBKDF2_ERROR,
#endif
#ifdef EVP_R_PKEY_APPLICATION_ASN1_METHOD_ALREADY_REGISTERED
pkey_application_asn1_method_already_registered = EVP_R_PKEY_APPLICATION_ASN1_METHOD_ALREADY_REGISTERED,
#endif
#ifdef EVP_R_PRIVATE_KEY_DECODE_ERROR
private_key_decode_error                        = EVP_R_PRIVATE_KEY_DECODE_ERROR,
#endif
#ifdef EVP_R_PRIVATE_KEY_ENCODE_ERROR
private_key_encode_error                        = EVP_R_PRIVATE_KEY_ENCODE_ERROR,
#endif
#ifdef EVP_R_PUBLIC_KEY_NOT_RSA
public_key_not_rsa                              = EVP_R_PUBLIC_KEY_NOT_RSA,
#endif
#ifdef EVP_R_TOO_LARGE
too_large                                       = EVP_R_TOO_LARGE,
#endif
#ifdef EVP_R_UNKNOWN_CIPHER
unknown_cipher                                  = EVP_R_UNKNOWN_CIPHER,
#endif
#ifdef EVP_R_UNKNOWN_DIGEST
unknown_digest                                  = EVP_R_UNKNOWN_DIGEST,
#endif
#ifdef EVP_R_UNKNOWN_OPTION
unknown_option                                  = EVP_R_UNKNOWN_OPTION,
#endif
#ifdef EVP_R_UNKNOWN_PBE_ALGORITHM
unknown_pbe_algorithm                           = EVP_R_UNKNOWN_PBE_ALGORITHM,
#endif
#ifdef EVP_R_UNSUPPORTED_ALGORITHM
unsupported_algorithm                           = EVP_R_UNSUPPORTED_ALGORITHM,
#endif
#ifdef EVP_R_UNSUPPORTED_CIPHER
unsupported_cipher                              = EVP_R_UNSUPPORTED_CIPHER,
#endif
#ifdef EVP_R_UNSUPPORTED_KEYLENGTH
unsupported_keylength                           = EVP_R_UNSUPPORTED_KEYLENGTH,
#endif
#ifdef EVP_R_UNSUPPORTED_KEY_DERIVATION_FUNCTION
unsupported_key_derivation_function             = EVP_R_UNSUPPORTED_KEY_DERIVATION_FUNCTION,
#endif
#ifdef EVP_R_UNSUPPORTED_KEY_SIZE
unsupported_key_size                            = EVP_R_UNSUPPORTED_KEY_SIZE,
#endif
#ifdef EVP_R_UNSUPPORTED_NUMBER_OF_ROUNDS
unsupported_number_of_rounds                    = EVP_R_UNSUPPORTED_NUMBER_OF_ROUNDS,
#endif
#ifdef EVP_R_UNSUPPORTED_PRF
unsupported_prf                                 = EVP_R_UNSUPPORTED_PRF,
#endif
#ifdef EVP_R_UNSUPPORTED_PRIVATE_KEY_ALGORITHM
unsupported_private_key_algorithm               = EVP_R_UNSUPPORTED_PRIVATE_KEY_ALGORITHM,
#endif
#ifdef EVP_R_UNSUPPORTED_SALT_TYPE
unsupported_salt_type                           = EVP_R_UNSUPPORTED_SALT_TYPE,
#endif
#ifdef EVP_R_WRAP_MODE_NOT_ALLOWED
wrap_mode_not_allowed                           = EVP_R_WRAP_MODE_NOT_ALLOWED,
#endif
#ifdef EVP_R_WRONG_FINAL_BLOCK_LENGTH
wrong_final_block_length                        = EVP_R_WRONG_FINAL_BLOCK_LENGTH,
#endif
#ifdef EVP_R_XTS_DATA_UNIT_IS_TOO_LARGE
xts_data_unit_is_too_large                      = EVP_R_XTS_DATA_UNIT_IS_TOO_LARGE,
#endif
#ifdef EVP_R_XTS_DUPLICATED_KEYS
xts_duplicated_keys                             = EVP_R_XTS_DUPLICATED_KEYS,
#endif
};

}


namespace std {
template <>
class is_error_code_enum<openssl::evp_errc> : std::true_type { };
}

namespace openssl {

inline std::error_code make_error_code(evp_errc errc)
{
    return std::error_code(static_cast<int>(errc), openssl_crypto_category());
}


enum class message_digest_algorithm
{
    md2,
    md4,
    md5,
    md5_sha1,
    blake2b512,
    blake2s256,
    sha1,
    sha224,
    sha256,
    sha384,
    sha512,
    sha512_224,
    sha512_256,
    sha3_224,
    sha3_256,
    sha3_384,
    sha3_512,
    shake128,
    shake256,
    mdc2,
    ripemd160,
    whirlpool,
    sm3,
};


namespace detail {

inline const ::EVP_MD* get_evp_message_digest(message_digest_algorithm algorithm)
{
    switch (algorithm) {
    case message_digest_algorithm::md2: {
#ifndef OPENSSL_NO_MD2
        return ::EVP_md2();
#endif
        return nullptr;
    }
    case message_digest_algorithm::md4: {
#ifndef OPENSSL_NO_MD4
        return ::EVP_md4();
#endif
        return nullptr;
    }
    case message_digest_algorithm::md5: {
#ifndef OPENSSL_NO_MD5
        return ::EVP_md5();
#endif
        return nullptr;
    }
    case message_digest_algorithm::md5_sha1: {
#ifndef OPENSSL_NO_MD5
        return ::EVP_md5_sha1();
#endif
        return nullptr;
    }
    case message_digest_algorithm::blake2b512: {
#ifndef OPENSSL_NO_BLAKE2
        return ::EVP_blake2b512();
#endif
        return nullptr;
    }
    case message_digest_algorithm::blake2s256: {
#ifndef OPENSSL_NO_BLAKE2
        return ::EVP_blake2s256();
#endif
        return nullptr;
    }
    case message_digest_algorithm::sha1:
        return ::EVP_sha1();
    case message_digest_algorithm::sha224:
        return ::EVP_sha224();
    case message_digest_algorithm::sha256:
        return ::EVP_sha256();
    case message_digest_algorithm::sha384:
        return ::EVP_sha384();
    case message_digest_algorithm::sha512:
        return ::EVP_sha512();
    case message_digest_algorithm::sha512_224:
        return ::EVP_sha512_224();
    case message_digest_algorithm::sha512_256:
        return ::EVP_sha512_256();
    case message_digest_algorithm::sha3_224:
        return ::EVP_sha3_224();
    case message_digest_algorithm::sha3_256:
        return ::EVP_sha3_256();
    case message_digest_algorithm::sha3_384:
        return ::EVP_sha3_384();
    case message_digest_algorithm::sha3_512:
        return ::EVP_sha3_512();
    case message_digest_algorithm::shake128:
        return ::EVP_shake128();
    case message_digest_algorithm::shake256:
        return ::EVP_shake256();
    case message_digest_algorithm::mdc2: {
#ifndef OPENSSL_NO_MDC2
        return ::EVP_mdc2();
#endif
        return nullptr;
    }
    case message_digest_algorithm::ripemd160: {
#ifndef OPENSSL_NO_RMD160
        return ::EVP_ripemd160();
#endif
        return nullptr;
    }
    case message_digest_algorithm::whirlpool: {
#ifndef OPENSSL_NO_WHIRLPOOL
        return ::EVP_whirlpool();
#endif
        return nullptr;
    }
    case message_digest_algorithm::sm3: {
# ifndef OPENSSL_NO_SM3
        return ::EVP_sm3();
#endif
        return nullptr;
    }
    default:
        return nullptr;
    }
}

} // namespace detail



class message_digest
{
public:
    explicit message_digest(message_digest_algorithm algorithm, ::ENGINE* engine = nullptr)
        : context_()
        , algorithm_context_(detail::get_evp_message_digest(algorithm))
        , engine_(engine)
    {
        context_ = ::EVP_MD_CTX_new();
        if (context_ == nullptr) {
            throw std::system_error((int)ERR_get_error(), openssl_crypto_category());
        }
        ::EVP_MD_CTX_init(context_);

        if (algorithm_context_ == nullptr) {
            throw std::system_error(make_error_code(evp_errc::unsupported_algorithm));
        }
        if (::EVP_DigestInit_ex(context_, algorithm_context_, engine_) != 1) {
            throw std::system_error((int)ERR_get_error(), openssl_crypto_category());
        }
    }

    void update(std::span<const std::byte> data)
    {
        ::EVP_DigestUpdate(context_, reinterpret_cast<const void*>(data.data()), data.size());
    }

    void finalize_to(std::span<std::byte> digest)
    {
        auto digest_size = static_cast<unsigned int>(digest.size());
        ::EVP_DigestFinal_ex(context_, reinterpret_cast<unsigned char*>(digest.data()), &digest_size);
    }

    void reset()
    {
        if (::EVP_DigestInit_ex(context_, algorithm_context_, engine_) != 1) {
            throw std::system_error((int)ERR_get_error(), openssl_crypto_category());
        }
    }

    ~message_digest()
    {
        ::EVP_MD_CTX_free(context_);
    };

private:
    ::EVP_MD_CTX* context_;
    const ::EVP_MD* algorithm_context_;
    ::ENGINE* engine_;
};

}