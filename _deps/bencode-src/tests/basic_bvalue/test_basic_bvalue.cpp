//
// Created by fbdtemme on 8/14/20.
//
#include <catch2/catch.hpp>

#include <string_view>

#include "bencode/traits/all.hpp"
#include "bencode/bvalue.hpp"


using namespace std::string_view_literals;
namespace bc = bencode;

TEST_CASE("test basic_bvalue members", "[bvalue]")
{
    const bencode::bvalue const_integer(1);
    const bencode::bvalue const_string("test");
    const bencode::bvalue const_list(bc::btype::list, {1, 2, 3});
    const bencode::bvalue const_dict(bc::btype::dict, {{"test1", 1}, {"test2", "2"}});

    bencode::bvalue integer(1);
    bencode::bvalue string("test");
    bencode::bvalue list(bc::btype::list, {1, 2, 3});
    bencode::bvalue dict(bc::btype::dict, {{"test1", 1}, {"test2", "2"}});

    SECTION("operator bool()") {
        CHECK(const_integer);
        CHECK(const_string);
        CHECK(const_list);
        CHECK(const_dict);
        CHECK_FALSE(bc::bvalue{});
    }
    SECTION("at()") {
        SECTION("at(int)") {
            CHECK(const_list.at(0) == 1);
            CHECK(list.at(0) == 1);
            CHECK_THROWS_AS(const_string.at(0), bc::bad_bvalue_access);
            CHECK_THROWS_AS(string.at(0), bc::bad_bvalue_access);
        }
        SECTION("at(string") {
            CHECK(const_dict.at("test1") == 1);
            CHECK(dict.at("test1") == 1);
            CHECK_THROWS_AS(const_string.at("test1"), bc::bad_bvalue_access);
            CHECK_THROWS_AS(string.at("test1"), bc::bad_bvalue_access);
        }
    }

    SECTION("operator[]") {
        SECTION("operator[](int)") {
            CHECK(const_list[0] == 1);
            CHECK(list[0] == 1);
            CHECK_THROWS_AS(const_integer[0], bc::bad_bvalue_access);
            CHECK_THROWS_AS(integer[0], bc::bad_bvalue_access);
        }
        const auto s = std::string("test5");

        SECTION("operator[](int)") {
            CHECK(dict["test1"] == 1);

            dict["test4"] = 4;
            dict[s] = 5;
            CHECK(dict["test4"] == 4);
            CHECK(dict[s] == 5);

            CHECK_THROWS_AS(integer["test1"], bc::bad_bvalue_access);
            CHECK_THROWS_AS(integer[s], bc::bad_bvalue_access);
        }
    }

    SECTION("front") {
        CHECK(const_list.front() == 1);
        list.front() = 2;
        CHECK(list.front() == 2);

        SECTION("error - not list type") {
            CHECK_THROWS_AS(integer.front(), bc::bad_bvalue_access);
            CHECK_THROWS_AS(const_integer.front(), bc::bad_bvalue_access);
        }
    }

    SECTION("back") {
        CHECK(const_list.back() == 3);
        list.back() = 2;
        CHECK(list.back() == 2);

        SECTION("error - not list type") {
            CHECK_THROWS_AS(integer.back(), bc::bad_bvalue_access);
            CHECK_THROWS_AS(const_integer.back(), bc::bad_bvalue_access);
        }
    }

    SECTION("push_back") {
        const bc::bvalue s_const = "long string to disable sso optimisation 1";
        std::string s = "long string to disable sso optimisation 2";

        SECTION("copy") {
            list.push_back(s_const);
            CHECK(list[3] == s_const);
            CHECK(s_const == "long string to disable sso optimisation 1");
        }
        SECTION("move") {
            list.push_back(std::move(s));
            CHECK(s.empty());
        }

        SECTION("error - not list type") {
            CHECK_THROWS_AS(integer.push_back("test1"), bc::bad_bvalue_access);
            CHECK_THROWS_AS(integer.push_back(s_const), bc::bad_bvalue_access);
        }
    }
    SECTION("emplace back") {
        auto l = bc::bvalue(bc::btype::list, {1, 2, 3});

        SECTION("copy") {
            list.emplace_back(l);
            CHECK(list[3]==l);
        }
        SECTION("move") {
            list.emplace_back(std::move(l));
            CHECK(get_list(l).empty());
        }
        SECTION("error - not list type") {
            CHECK_THROWS_AS(integer.emplace_back(l), bc::bad_bvalue_access);
        }
    }

    SECTION("contains") {
        const std::string s = "test1";
        CHECK(dict.contains(s));
        CHECK(dict.contains("test1"));

        SECTION("error - not dict type") {
            CHECK_THROWS_AS(integer.contains("test1"), bc::bad_bvalue_access);
            CHECK_THROWS_AS(integer.contains(s), bc::bad_bvalue_access);
        }
    }

    SECTION("clear") {
        auto b = bc::bvalue();
        b.clear();
        CHECK_FALSE(b);

        integer.clear();
        CHECK(get_integer(integer) == 0);

        string.clear();
        CHECK(get_string(string).empty());

        list.clear();
        CHECK(get_list(list).empty());

        dict.clear();
        CHECK(get_dict(dict).empty());
    }
}