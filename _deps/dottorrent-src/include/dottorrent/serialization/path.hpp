#pragma once
#include <filesystem>
#include <bencode/bencode.hpp>

BENCODE_SERIALIZES_TO_LIST(std::filesystem::path);

// We define this in bencode namespace since it we prefer that to defining this in std namespace
// so it can be found by ADL.
namespace bencode {

template <bencode::event_consumer EC>
void bencode_connect(
        bencode::customization_point_type<std::filesystem::path>,
        EC& consumer,
        const std::filesystem::path& value)
{
    consumer.list_begin();
    for (auto& v: value) {
        consumer.string(v.generic_string());
        consumer.list_item();
    }
    consumer.list_end();
}

}
