#pragma once

#include "bencode/detail/bad_access.hpp"

namespace bencode {

/// Exception thrown when accessing a basic_bvalue as a type that does not match the
/// currently active alternative.
class bad_bvalue_access : public bad_access
{
    using bad_access::bad_access;
};
} // namespace bencode