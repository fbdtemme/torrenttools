//
// Created by fbdtemme on 12/01/2020.
//
#include <catch2/catch.hpp>
#include <type_traits>
#include <string_view>

#include <termcontrol/termcontrol.hpp>

using namespace std::string_view_literals;


TEST_CASE("test constrol_sequence - cursor up")
{
    using cursor_up = termcontrol::control_sequence<termcontrol::definitions::cursor_up>;
    CHECK(cursor_up::max_size() == 8);

    SECTION("construction - one explicit argument") {
        CHECK((cursor_up(20) <=> "\033[20A") == std::weak_ordering::equivalent);
        CHECK(cursor_up(20) == "\033[20A");
    }

    SECTION("constuction - tag type")
    {
        auto cs = termcontrol::control_sequence(termcontrol::ecma48::cursor_up, 10);
    }

    SECTION("construction - default argument") {
        CHECK(cursor_up() == "\033[1A");
    }

    auto s = cursor_up(20);

    SECTION("c_str()") {
        const char* cs = s.c_str();
        CHECK(strcmp(cs, "\033[20A") == 0);
    }

    SECTION("str()") {
        std::string ss = s.str();
        CHECK(ss == "\033[20A");
    }

    SECTION("conversion to string through implicit string_view") {
        auto ss = std::string(s);
        CHECK(ss == "\033[20A");
    }
}
