
#include <sstream>
#include "bencode/detail/serialization_traits.hpp"

namespace bencode {

template <>
struct serialization_traits<std::stringstream>     : serializes_to_string {};

}
