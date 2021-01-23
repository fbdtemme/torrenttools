#include <catch2/catch.hpp>
#include "bencode/bview.hpp"
#include "data.hpp"

using namespace bencode;

//
// Created by fbdtemme on 8/14/20.
//
#include <catch2/catch.hpp>

#include <string_view>

#include "bencode/traits/all.hpp"
#include "bencode/bvalue.hpp"


using namespace std::string_view_literals;
namespace bc = bencode;

TEST_CASE("test basic_bview members", "[bview]")
{
    SECTION("operator bool()") {
        CHECK(i_view_const);
        CHECK(s_view_const);
        CHECK(l_view_const);
        CHECK(d_view_const);
        CHECK_FALSE(bview{});
    }
    SECTION("at()") {
        SECTION("at(int)") {
            CHECK(l_view_const.at(0) == 2);
            CHECK(l_view_const.at(0) == 2);
            CHECK_THROWS_AS(l_view_const.at(1000), bc::out_of_range);
            CHECK_THROWS_AS(s_view_const.at(0), bc::bad_bview_access);
            CHECK_THROWS_AS(s_view_const.at(0), bc::bad_bview_access);
        }
        SECTION("at(string_view") {
            CHECK(d_view_const.at("spam") == 1);
            CHECK(d_view_const.at("spam") == 1);
            CHECK_THROWS_AS(d_view_const.at("bar"), bc::out_of_range);
            CHECK_THROWS_AS(i_view_const.at("spam"), bc::bad_bview_access);
            CHECK_THROWS_AS(s_view_const.at("spam"), bc::bad_bview_access);
        }
    }

    SECTION("operator[]") {
        SECTION("operator[](int)") {
            CHECK(l_view_const[0] == 2);
            CHECK(l_view_const[0] == 2);
            CHECK_THROWS_AS(i_view_const[0], bc::bad_bview_access);
            CHECK_THROWS_AS(i_view_const[0], bc::bad_bview_access);
        }
        const auto s = std::string("test5");
    }

    SECTION("front") {
        CHECK(l_view_const.front() == 2);

        SECTION("error - not list type") {
            CHECK_THROWS_AS(i_view_const.front(), bc::bad_bview_access);
            CHECK_THROWS_AS(s_view_const.front(), bc::bad_bview_access);
        }
    }

    SECTION("back") {
        CHECK(l_view_const.back() == 3);

        SECTION("error - not list type") {
            CHECK_THROWS_AS(i_view_const.back(), bc::bad_bview_access);
            CHECK_THROWS_AS(s_view_const.back(), bc::bad_bview_access);
        }
    }

    SECTION("contains") {
        const std::string s = "spam";
        CHECK(d_view_const.contains(s));
        CHECK_FALSE(d_view_const.contains("bar"));

        SECTION("error - not dict type") {
            CHECK_THROWS_AS(i_view_const.contains("test1"), bc::bad_bview_access);
            CHECK_THROWS_AS(i_view_const.contains(s), bc::bad_bview_access);
        }
    }
}