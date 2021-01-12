#include <catch2/catch.hpp>
#include <iostream>
//#include <detail/format.hpp>

#include "utils.hpp"

TEST_CASE("test parse_file ls_colors")
{
    auto map = load_ls_style_map();

    for (auto [k, v] : map) {
        std::cout << termcontrol::format(v, "{}\n", k);
    }
}