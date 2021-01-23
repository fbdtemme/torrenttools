#ifdef DOTTORRENT_USE_WINCNG
#include <system_error>
#include "dottorrent/hasher/backends/wincng.hpp"

namespace wincng {

const std::error_category& wincng_category()
{
    static detail::wincng_category instance{};
    return instance;
}

}
#endif