//#include <catch2/catch.hpp>
//
//#include "cliprogressbar/widgets/bar.hpp"
//#include <iostream>
//#include <sstream>
//
//
//TEST_CASE("bar")
//{
//    cliprogress::bar_style style{};
//    cliprogress::bar_symbols symbols{};
//
//    cliprogress::bar bar(symbols, style);
//    bar.allocate_size(bar.natural_size());
//    bar.set_percentage(40);
//    std::stringstream os {};
//
//    os << bar;
//    auto r = os.str();
//    std::cout << r << std::endl;
//    SUCCEED();
//}