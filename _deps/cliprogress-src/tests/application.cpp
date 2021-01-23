//
//#include <catch2/catch.hpp>
//
//#include <chrono>
//#include <thread>
//#include "cliprogressbar/widgets/bar.hpp"
//#include "cliprogressbar/application.hpp"
//
//#include <iostream>
//#include <sstream>
//
//
//TEST_CASE("application")
//{
//    using namespace std::chrono_literals;
//    cliprogress::application app{};
//    auto bar = std::make_shared<cliprogress::bar>(
//            cliprogress::bar_symbols{}, cliprogress::bar_style{});
//    app.set(bar);
//
//    auto t = std::jthread([&]() {
//        std::this_thread::sleep_for(10s);
//        app.request_stop();
//    });
//    app.start();
//    SUCCEED();
//}
//
//
