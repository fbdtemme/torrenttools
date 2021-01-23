
#include <type_traits>
#include <system_error>
#include <span>
#include <string>

#include <windows.h>
#include <bcrypt.h>
#include <ntstatus.h>

namespace wincng {

enum wincng_errc : std::int64_t
{
    success = STATUS_SUCCESS,                      // The function was successful.
    not_found = STATUS_NOT_FOUND,                  //     No provider was found for the specified algorithm ID.
    invalid_parameter = STATUS_INVALID_PARAMETER,  //    One or more parameters are not valid.
    no_memory = STATUS_NO_MEMORY,                  //     A memory allocation failure occurred.
    buffer_to_small = STATUS_BUFFER_TOO_SMALL,     // The buffer size specified by the cbOutput parameter is not large enough to hold the property value.
    invalid_handle = STATUS_INVALID_HANDLE,        // The handle in the hObject parameter is not valid.
    not_supported = STATUS_NOT_SUPPORTED,          //     The named property specified by the pszProperty parameter is not supported.
};


namespace detail {

class wincng_category : public std::error_category
{
public:
    wincng_category() = default;

    const char* name() const noexcept override
    {
        return "wincng";
    }

    std::string message(int value) const override
    {
        auto ec = wincng_errc(value);
        switch (ec) {
        case wincng_errc::success:
            return "The function was successful.";
        case wincng_errc::not_found:
            return "No provider was found for the specified algorithm ID.";
        case wincng_errc::invalid_parameter:
            return "One or more parameters are not valid.";
        case wincng_errc::no_memory:
            return "A memory allocation failure occurred.";
        case wincng_errc::buffer_to_small:
            return "The buffer size specified by the cbOutput parameter is not large enough to hold the property value.";
        case wincng_errc::invalid_handle:
            return "The handle in the hObject parameter is not valid.";
        case wincng_errc::not_supported:
            return "The named property specified by the pszProperty parameter is not supported.";
        default:
            return "Unkown error";
        }
    }
};

} //namespace detail

const std::error_category& wincng_category();


enum class message_digest_algorithm : std::uint64_t
{
    md2,
    md4,
    md5,
    sha1,
    sha256,
    sha384,
    sha512
};

static constexpr const wchar_t* get_bcrypt_message_digest_algorithm(message_digest_algorithm algo)
{
    switch(algo) {
    case message_digest_algorithm::md2:   return BCRYPT_MD2_ALGORITHM;
    case message_digest_algorithm::md4:   return BCRYPT_MD4_ALGORITHM;
    case message_digest_algorithm::md5:   return BCRYPT_MD5_ALGORITHM;
    case message_digest_algorithm::sha1:  return BCRYPT_SHA1_ALGORITHM;
    case message_digest_algorithm::sha256: return BCRYPT_SHA256_ALGORITHM;
    case message_digest_algorithm::sha384: return BCRYPT_SHA384_ALGORITHM;
    case message_digest_algorithm::sha512: return BCRYPT_SHA512_ALGORITHM;
    default:
        throw std::invalid_argument("unsopported algorithm by wincng backend");
    }
}



} // namespace wincng


namespace std {
template <>
class is_error_code_enum<wincng::wincng_errc> : std::true_type { };
}

namespace wincng {

inline std::error_code make_error_code(wincng_errc errc)
{
    return std::error_code(static_cast<int>(errc), wincng_category());
}


class bcrypt_algorithm_provider
{
public:
    explicit bcrypt_algorithm_provider(const wchar_t* algorithm_identifier)
    {
        // Open the algorithm provider context
        int status = ::BCryptOpenAlgorithmProvider(
                &algorithm_handle_,
                algorithm_identifier,
                nullptr,
                BCRYPT_HASH_REUSABLE_FLAG);

        if (!BCRYPT_SUCCESS(status)) {
            throw std::system_error(make_error_code(wincng_errc(status)));
        }
    }

    ULONG hash_object_size() {
        ULONG hash_object_size;
        ULONG bytes_coped;

        int status = ::BCryptGetProperty(
                algorithm_handle_,
                BCRYPT_OBJECT_LENGTH,
                (PBYTE) &hash_object_size,
                sizeof(ULONG),
                &bytes_coped,
                0);

        if (!BCRYPT_SUCCESS(status)) {
            throw std::system_error(make_error_code(wincng_errc(status)));
        }

        return hash_object_size;
    }

    ULONG hash_size()
    {
        ULONG hash_size;
        ULONG bytes_coped;

        int status = BCryptGetProperty(
                algorithm_handle_,
                BCRYPT_HASH_LENGTH,
                (PBYTE) &hash_size,
                sizeof(ULONG),
                &bytes_coped,
                0);

        if (!BCRYPT_SUCCESS(status)) {
            throw std::system_error(make_error_code(wincng_errc(status)));
        }
        return hash_size;
    }

    ::BCRYPT_ALG_HANDLE handle() noexcept {
        return algorithm_handle_;
    }

    ~bcrypt_algorithm_provider()
    {
        ::BCryptCloseAlgorithmProvider(algorithm_handle_, 0);
    }

private:
    ::BCRYPT_ALG_HANDLE algorithm_handle_;
};


class message_digest
{
public:
    explicit message_digest(message_digest_algorithm algorithm)
            : context_()
            , algorithm_provider_(get_bcrypt_message_digest_algorithm(algorithm))
    {
        // allocate the hasher object on the heap, pbHashObject
        ULONG hash_object_size_ = algorithm_provider_.hash_object_size();
        hasher_object_ = new BYTE[hash_object_size_];
        if (hasher_object_ == nullptr) {
            throw std::bad_alloc();
        }

        // allocate the hasher buffer on the heap
        hasher_buffer_ = new BYTE[algorithm_provider_.hash_size()];
        if (hasher_object_ == nullptr) {
            throw std::bad_alloc();
        }

        //create a hash
        int status = BCryptCreateHash(
                algorithm_provider_.handle(),
                &context_,
                hasher_object_,
                hash_object_size_,
                nullptr,
                0,
                BCRYPT_HASH_REUSABLE_FLAG);

        if (!BCRYPT_SUCCESS(status)) {
            throw std::system_error(make_error_code(wincng_errc(status)));
        }
    }

    void update(std::span<const std::byte> data)
    {
        //hash some data
        int status = ::BCryptHashData(context_, (PBYTE) data.data(), data.size(), 0);
        if (!BCRYPT_SUCCESS(status)) {
            throw std::system_error(make_error_code(wincng_errc(status)));
        }
    }

    void finalize_to(std::span<std::byte> digest)
    {
        int status = ::BCryptFinishHash(context_, (PBYTE) digest.data(), digest.size(), 0);
        if (!BCRYPT_SUCCESS(status)) {
            throw std::system_error(make_error_code(wincng_errc(status)));
        }
    }

    void reset()
    { }

    ~message_digest()
    {
        delete[] hasher_object_;
        delete[] hasher_buffer_;
    };

private:
    ::PBYTE hasher_object_ = nullptr;
    ::PBYTE hasher_buffer_ = nullptr;
    ::BCRYPT_HASH_HANDLE context_;
    bcrypt_algorithm_provider algorithm_provider_;
};
}
