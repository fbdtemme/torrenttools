#ifdef DOTTORRENT_USE_OPENSSL
#include <system_error>
#include "dottorrent/hasher/backends/openssl.hpp"

namespace openssl {

const std::error_category& openssl_crypto_category()
{
    static detail::openssl_crypto_category instance{};
    return instance;
}

}
#endif