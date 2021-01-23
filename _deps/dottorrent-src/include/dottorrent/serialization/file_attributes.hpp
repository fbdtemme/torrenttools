#pragma once
#include <bencode/bencode.hpp>
#include <dottorrent/file_entry.hpp>
#include <dottorrent/general.hpp>

BENCODE_SERIALIZES_TO_LIST(dottorrent::file_attributes);

namespace dottorrent {

template <bencode::event_consumer EC>
constexpr void bencode_connect(
        bencode::customization_point_type<dottorrent::file_attributes>,
        EC& consumer,
        const dottorrent::file_attributes& value)
{
    consumer.string(to_string(value));
}

} // namespace dottorrent