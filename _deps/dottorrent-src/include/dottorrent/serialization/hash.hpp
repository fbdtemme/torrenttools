#pragma once

#include <type_traits>
#include <concepts>

#include <bencode/bencode.hpp>
#include "dottorrent/hash.hpp"

BENCODE_SERIALIZES_TO_STRING(dottorrent::sha1_hash)
BENCODE_SERIALIZES_TO_STRING(dottorrent::sha256_hash)
BENCODE_SERIALIZES_TO_STRING(dottorrent::md5_hash)

namespace dottorrent {

template <bencode::event_consumer EC, hash_type T>
constexpr void bencode_connect(bencode::customization_point_type<T>,
                               EC& consumer,
                               const T& value)
{
    consumer.string(std::string_view(value));
}

}
