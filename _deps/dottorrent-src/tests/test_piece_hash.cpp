////
//// Created by fbdtemme on 11/02/19.
////
//
#include <catch2/catch.hpp>
#include <iostream>

#include "dottorrent/hash.hpp"


TEST_CASE("test construction", "[sha1]") {
    using namespace dottorrent;
    sha1_hash hash{};
}