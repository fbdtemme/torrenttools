#pragma once

#pragma once

#include <memory>
#include "bencode/detail/serialization_traits.hpp"

namespace bencode {

template <bencode::serializable T, typename D>
struct serialization_traits<std::unique_ptr<T, D>> : serialization_traits<T>
{
    static constexpr auto is_pointer = true;
};

template <bencode::serializable T>
struct serialization_traits<std::shared_ptr<T>> : serialization_traits<T>
{
    static constexpr auto is_pointer = true;
};

template <bencode::serializable T>
struct serialization_traits<std::weak_ptr<T>> : serialization_traits<T>
{
    static constexpr auto is_pointer = true;
};


}

