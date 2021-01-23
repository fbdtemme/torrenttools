#pragma once

#include <stdexcept>

namespace dottorrent {

class bad_torrent_file : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

struct parse_error : std::runtime_error
{
    using std::runtime_error::runtime_error;

    parse_error(std::string_view key, std::string_view msg)
            : std::runtime_error(
            fmt::format("parse error for field \"{}\": {}", key, msg))
    {};

};

}