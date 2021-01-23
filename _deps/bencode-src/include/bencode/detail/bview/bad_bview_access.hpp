#pragma once

#include "bencode/detail/bad_access.hpp"

namespace bencode {

/// Exception thrown when trying to access a bview with a type that does not match the
/// currently active alternative.
class bad_bview_access : public bad_access {
public:
    using bad_access::bad_access;
};

}
