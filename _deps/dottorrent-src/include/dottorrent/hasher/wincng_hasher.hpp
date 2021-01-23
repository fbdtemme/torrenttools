#pragma once

#include "dottorrent/hash_function.hpp"
#include "dottorrent/hasher/hasher.hpp"
#include "dottorrent/hasher/backends/wincng.hpp"

namespace dottorrent {

namespace detail {

static const auto supported_hash_functions_wincng = std::unordered_set {
        hash_function::md4,
        hash_function::md5,
        hash_function::sha1,
        hash_function::sha256,
        hash_function::sha384,
        hash_function::sha512,
};

constexpr wincng::message_digest_algorithm get_wincng_algorithm(hash_function function)
{
    using namespace wincng;

    switch (function) {
        case hash_function::md4:          return message_digest_algorithm::md4;
        case hash_function::md5:          return message_digest_algorithm::md5;
        case hash_function::sha1:         return message_digest_algorithm::sha1;
        case hash_function::sha256:       return message_digest_algorithm::sha256;
        case hash_function::sha384:       return message_digest_algorithm::sha384;
        case hash_function::sha512:       return message_digest_algorithm::sha512;

        default:
            throw std::invalid_argument("No matching wincng algorithm.");
    }
}
}


class wincng_hasher : public hasher
{
public:
    explicit wincng_hasher(hash_function function)
            : digest_(detail::get_wincng_algorithm(function))
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
        return detail::supported_hash_functions_wincng;
    }

private:
    wincng::message_digest digest_;
};



} // namespace dottorrent