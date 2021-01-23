//
// Created by fbdtemme on 03/06/19.
//
#include <catch2/catch.hpp>
#include <sstream>
#include <bencode/detail/bencode_type.hpp>
//#include "../bencode/operators.hpp"



using namespace Catch;
using namespace Catch::Generators;
using bencode::bencode_type;



TEST_CASE("test string conversion", "[type]")
{
    auto [input, expected] = GENERATE(table<bencode::bencode_type, std::string>({
        {bencode_type::uninitialized, "uninitialized"},
        {bencode_type::integer,       "integer"},
        {bencode_type::string,        "string"},
        {bencode_type::list,          "list"},
        {bencode_type::dict,          "dict"}
    }));

    SECTION("to_string") {
        CHECK(bencode::to_string(input) == expected);
    }
    SECTION("operator<<") {
        std::stringstream s {};
        s << input;
        CHECK(s.str() == expected);
    }
}

TEST_CASE("check comparison operators", "[type]")
{
    static std::array types = {
            bencode_type::uninitialized,
            bencode_type::integer,
            bencode_type::string,
            bencode_type::list, bencode_type::dict
    };

    auto i = GENERATE(range(0, 4));
    int j = 0;

    for (; j < i; ++j) {
        CHECK(types[j] != types[i]);
        CHECK_FALSE(types[j] == types[i]);

        CHECK(types[j] < types[i]);
        CHECK(types[j] <= types[i]);
        CHECK_FALSE(types[j] >= types[i]);
        CHECK_FALSE(types[j] > types[i]);
    }
    CHECK_FALSE(types[j] != types[i]);
    CHECK(types[j] == types[i]);

    CHECK_FALSE(types[j] < types[i]);
    CHECK(types[j] <= types[i]);
    CHECK(types[j] >= types[i]);
    CHECK_FALSE(types[j] > types[i]);
    j++;

    for (; j < std::size(types); ++j) {
        CHECK(types[j] != types[i]);
        CHECK_FALSE(types[j] == types[i]);

        CHECK_FALSE(types[j] < types[i]);
        CHECK_FALSE(types[j] <= types[i]);
        CHECK(types[j] >= types[i]);
        CHECK(types[j] > types[i]);
    }
}