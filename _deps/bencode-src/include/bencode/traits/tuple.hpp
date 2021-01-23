#pragma once

#include <tuple>
#include "bencode/detail/serialization_traits.hpp"

namespace bencode {

template <typename... Ts>
struct serialization_traits<std::tuple<Ts...>> : serializes_to_list {};

}