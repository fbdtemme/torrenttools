#include <catch2/catch.hpp>
#include <dottorrent/file_entry.hpp>


TEST_CASE("attributes to string")
{
    using namespace dottorrent;

    SECTION("single attribute") {
        auto s = to_string(file_attributes::padding_file);
        CHECK(s == "p");
    }

    SECTION("multiple attribute") {
        auto s = to_string(file_attributes::executable | file_attributes::hidden);
        CHECK(s == "xh");
    }
}
