//
// Created by fbdtemme on 10/4/20.
//

#include <catch2/catch.hpp>
#include "dottorrent/checksum.hpp"

namespace dt = dottorrent;

TEST_CASE("generic checksum")
{
    SECTION("default construction - sha1")
    {
        auto zero_sha1 = dt::sha1_hash{};
        auto checksum = make_checksum(dt::hash_function::sha1);
    }
    SECTION("default construction - md5")
    {
        auto zero_sha1 = dt::md5_hash{};
        auto checksum = make_checksum(dt::hash_function::md5);
    }
    SECTION("default construction - sha256")
    {
        auto zero_sha1 = dt::sha256_hash{};
        auto checksum = make_checksum(dt::hash_function::sha256);
    }
    SECTION("default construction - sha512")
    {
        auto zero_sha1 = dt::sha512_hash{};
        auto checksum = make_checksum(dt::hash_function::sha512);
    }


}