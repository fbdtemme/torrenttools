#pragma once

#include <stack>
#include <string>
#include <string_view>
#include <optional>

#include <bencode/detail/bvalue/basic_bvalue.hpp>
#include <bencode/detail/events/concepts.hpp>

namespace bencode::detail {

template <typename Policy, typename U, event_consumer EC>
constexpr void connect_events_runtime_impl(
        customization_point_type<basic_bvalue<Policy>>,
        EC& consumer,
        U&& value,
        priority_tag<1>)
{
    using vt = bencode::bencode_type;
    switch (value.type()) {
        case vt::uninitialized: break;
        case vt::integer: {
            consumer.integer(get_integer(value));
            break;
        }
        case vt::string: {
            consumer.string(get_string(value));
            break;
        }
        case vt::list: {
            const auto& list = get_list(value);
            std::size_t size = list.size();
            consumer.list_begin(size);
            for (const auto& v : get_list(value)) {
                connect(consumer, v);
                consumer.list_item();
            }
            consumer.list_end();
            break;
        }
        case vt::dict: {
            const auto& dict = get_dict(value);
            std::size_t size = dict.size();
            consumer.dict_begin(size);
            for (const auto&[k, v] : dict) {
                consumer.string(k);
                consumer.dict_key();
                connect(consumer, v);
                consumer.dict_value();
            }
            consumer.dict_end();
            break;
        }
    }
}

} // bencode::detail