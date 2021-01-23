#pragma once
#include "bencode/detail/exception.hpp"

namespace bencode {

/// Base class for bad_bvalue_access and bad_bview_access exceptions.
class bad_access : public exception {
public:
    using exception::exception;
};

}