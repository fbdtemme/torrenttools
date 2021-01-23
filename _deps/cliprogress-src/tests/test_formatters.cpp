//
// Created by fbdtemme on 5/7/20.
//
#include <catch2/catch.hpp>
#include "cliprogress/detail/formatters.hpp"

using namespace cliprogress;

TEST_CASE("metric unit formatting")
{
    SECTION("test1") {
        std::string r = format_metric_unit(30, "m");
        CHECK(r == "30 m");
    }

    SECTION("test2") {
        std::string r {};
        r = format_metric_unit(3023, "m");
        CHECK(r == "3.02 km");
        r = format_metric_unit(3023, "m", 1);
        CHECK(r == "3.0 km");
    }
}

TEST_CASE("duration formatting unit formatting")
{
    SECTION("test1") {
        std::string r = format_metric_unit(30, "m");
        CHECK(r == "30 m");
    }

    SECTION("test2") {
        std::string r {};
        r = format_metric_unit(3023, "m");
        CHECK(r == "3.02 km");
        r = format_metric_unit(3023, "m", 1);
        CHECK(r == "3.0 km");
    }
}