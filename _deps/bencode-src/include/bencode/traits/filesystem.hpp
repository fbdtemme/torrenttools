#pragma once
#include <filesystem>
#include "bencode/detail/serialization_traits.hpp"

namespace bencode {

template<>
struct serialization_traits<std::filesystem::path> : serializes_to_string { };

} // namespace bencode