#include <catch2/catch_all.hpp>
#include <iostream>
//#include <detail/format.hpp>

#include "ls_colors.hpp"

TEST_CASE("test parse_file ls_colors")
{
    auto map = ls_colors();
    SUCCEED();
}