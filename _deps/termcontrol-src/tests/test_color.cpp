//
// Created by fbdtemme on 11/01/2020.
//
#include <catch2/catch.hpp>
#include <type_traits>

#include <termcontrol/termcontrol.hpp>

TEST_CASE("test text_style")
{
//    using namespace termcontrol;

    SECTION("default constuction") {
        auto t = termcontrol::text_style();
        CHECK_FALSE(t.has_emphasis());
        CHECK_FALSE(t.has_foreground_color());
        CHECK_FALSE(t.has_background_color());
    }
    SECTION("construction from fg / bg / em helpers") {
        auto t = em(termcontrol::emphasis::overline) | fg(termcontrol::terminal_color::red);
        CHECK(t.has_emphasis());
        CHECK(t.has_foreground_color());

        auto t2 = em(termcontrol::emphasis::overline) | bg(termcontrol::terminal_color::red);
        CHECK(t2.has_emphasis());
        CHECK(t2.has_background_color());
    }
}