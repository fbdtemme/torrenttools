#include <catch2/catch.hpp>
#include <limits>

#include <termcontrol/termcontrol.hpp>

using namespace termcontrol;


TEST_CASE("test control_sequence_parameter")
{
    CHECK(detail::max_formatted_size_v<erase_line_mode> == 1);
    CHECK(detail::max_formatted_size_v<erase_page_mode> == 1);
    CHECK(detail::max_formatted_size_v<tabulation_clear_mode> == 1);
    CHECK(detail::max_formatted_size_v<mode> == 2);
    CHECK(detail::max_formatted_size_v<dec_mode> == 4);
}

