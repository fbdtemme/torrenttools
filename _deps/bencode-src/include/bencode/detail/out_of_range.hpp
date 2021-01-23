#pragma once
#include "bencode/detail/exception.hpp"

namespace bencode {

/// Exception thrown when decoding integer values that do not fit in the destination data type.
class out_of_range : exception {
public:
    using exception::exception;
};

}