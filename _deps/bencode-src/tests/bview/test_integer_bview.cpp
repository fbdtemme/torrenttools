#include <string_view>
#include <catch2/catch.hpp>

#include "bencode/bview.hpp"
#include "data.hpp"

using namespace std::string_literals;

namespace bc = bencode;


TEST_CASE("test integer_bview") {
    auto integer = bc::integer_bview(descriptors_integer.data(), data_integer.data());

    SECTION("copy constructor") {
        bc::integer_bview bv = integer;
        CHECK(bv == integer);
    }
    SECTION("construction from bview") {
        auto s = bc::integer_bview(i_view);
        CHECK(s == i_view);
    }
    SECTION("bencoded_view") {
        CHECK(integer.bencoded_view() == data_integer);
    }
    SECTION("value()") {
        CHECK(integer.value() == 63);
    }

    SECTION("comparison with bview") {
        CHECK(i_view == integer);
        CHECK_FALSE(i_view != integer);
        CHECK(i_view <= integer);
        CHECK(i_view >= integer);
    }

    SECTION("comparison with integer value")
    {
        auto v = 63;

        CHECK(i_view == v);
        CHECK_FALSE(i_view != v);
        CHECK(i_view < 100);
        CHECK(i_view <= v);
        CHECK(i_view >= v);
        CHECK(i_view > 0);
    }

    SECTION("comparison with other bview types")
    {
        CHECK(i_view < s_view);
        CHECK(i_view < l_view);
        CHECK(i_view < d_view);
    }
}
