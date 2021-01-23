#ifdef DOTTORRENT_USE_GCRYPT
#include <system_error>
#include "dottorrent/hasher/backends/gcrypt.hpp"

namespace gcrypt {

const std::error_category& gcrypt_category()
{
    static detail::gcrypt_category instance{};
    return instance;
}

}
#endif