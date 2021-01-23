//
// Created by fbdtemme on 08/06/19.
//<

#include <catch2/catch.hpp>
#include <bencode/detail/itoa.hpp>
#include <string>


TEST_CASE("test integer to char") {
    using namespace std::string_literals;
    char buf[20];

    SECTION("uint32_t") {
        auto i = GENERATE(values<uint32_t>({0, 5, 10, 10, 9999, 123231}));

        char* end = inttostr::u32toa(i, buf);
        *end = '\0';
        CHECK(buf == std::to_string(i));
    }


    SECTION("int32_t") {
        auto i = GENERATE(values<int32_t>({
            -94, -24, -1, 0, 1, 10, 123231, 999999990
        }));

        char* end = inttostr::i32toa(i, buf);
        *end = '\0';
        CHECK(buf == std::to_string(i));
    }
}
