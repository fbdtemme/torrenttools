//
// Created by fbdtemme on 8/24/20.
//

#include <catch2/catch.hpp>
#include <unordered_map>
#include <map>
#include <iostream>


#include "bencode/bencode.hpp"
#include "bencode/events/encode_json_to.hpp"
#include "bencode/traits/all.hpp"
#include <fmt/format.h>

const auto path_result = fmt::format("\"{}\"", std::filesystem::current_path().string());
constexpr auto string_result = R"("abc")";
constexpr auto vector_result =
R"([
    1,
    2,
    3
])";

constexpr auto map_result =
R"({
    "a": 3,
    "b": 2,
    "c": 1
})";

constexpr auto pair_result =
R"([
    1,
    "2"
])";

using namespace bencode;

TEST_CASE("test connect")
{
    std::stringstream ss {};
    auto consumer = events::encode_json_to(ss);

    SECTION("std::vector<char>") {
        std::vector<char> lst {'a', 'b', 'c'};
        connect(consumer, lst);
        CHECK(ss.str() == string_result);
    }

    SECTION("std::filesystem::path") {
        auto p = std::filesystem::current_path();
        connect(consumer, p);
        CHECK(ss.str() == path_result);
    }
    SECTION("std::vector<std::byte>") {
        std::vector<std::byte> bytestr {std::byte('a'), std::byte('b'), std::byte('c')};
        connect(consumer, bytestr);
        CHECK(ss.str() == string_result);
    }
    SECTION("std::stringstream>") {
        std::stringstream strstr;
        strstr << "abc";
        connect(consumer, strstr);
        CHECK(ss.str() == string_result);
    }
    SECTION("std::tuple") {
        std::tuple<int, int, int> tpl {1, 2, 3};
        connect(consumer, tpl);
        CHECK(ss.str() == vector_result);
    }
    SECTION("std::pair") {
        std::pair<int, std::string> pair {1, "2"};
        connect(consumer, pair);
        CHECK(ss.str() == pair_result);
    }
    SECTION("std::array") {
        std::array<int, 3> arr {1, 2, 3};
        connect(consumer, arr);
        CHECK(ss.str() == vector_result);
    }
    SECTION("vector") {
        std::vector vctr {1, 2, 3};
        connect(consumer, vctr);
        CHECK(ss.str() == vector_result);
    }

    SECTION("map") {
        std::map<std::string, int> map{{"c", 1},
                                        {"b", 2},
                                        {"a", 3}};

        connect(consumer, map);
        CHECK(ss.str() == map_result);
    }

    SECTION("unordered_map") {
        std::unordered_map<std::string, int> umap{{"c", 1},
                                                  {"b", 2},
                                                  {"a", 3}};
        std::stringstream ss {};
        auto consumer = events::encode_json_to(ss);
        connect(consumer, umap);
        CHECK(ss.str() == map_result);
    }

}