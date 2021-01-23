#include <catch2/catch.hpp>
#include <dottorrent/hex.hpp>

#include <iostream>

namespace dt = dottorrent;

TEST_CASE("to_hexadecimal_string")
{
    std::vector<std::byte> data = {std::byte{0x0A}, std::byte{0x0B}, std::byte{0x0C}};
    auto r = dt::to_hexadecimal_string(data);
    CHECK(r == "0a0b0c");
}

TEST_CASE("from_hexadecimal_string")
{
    std::string hexdata = "303132";
    std::vector<std::byte> data {};

    SECTION("to output iterator") {
        dt::from_hexadecimal_string(std::back_inserter(data), hexdata);
        CHECK(data == std::vector{std::byte{48}, std::byte{49}, std::byte{50}});
    }
    SECTION("to static array") {
        auto data = dt::from_hexadecimal_string<3>(hexdata);
        CHECK(data == std::array{std::byte{48}, std::byte{49}, std::byte{50}});
    }
}

