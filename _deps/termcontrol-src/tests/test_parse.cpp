//
// Created by fbdtemme on 25/01/2020.
//

#include <catch2/catch.hpp>

#include <termcontrol/termcontrol.hpp>

TEST_CASE("test parse")
{
    auto s = termcontrol::total_control_sequences_length("Test\033[2m]BlahBlah\033[1AEnd");
    CHECK(s == 8);
}