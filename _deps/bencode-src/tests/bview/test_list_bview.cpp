
#include <type_traits>
#include <string>
#include <string_view>
#include <catch2/catch.hpp>

#include "bencode/bview.hpp"
#include "data.hpp"

using namespace std::string_literals;
namespace bc = bencode;



TEST_CASE("test list_bview") {
    bc::list_bview list(begin(descriptors_list), data_list.data());
    bc::list_bview nested_list(begin(descriptors_nested_list), data_nested_list.data());
    bc::list_bview list_reverse = bc::list_bview(&descriptors_list[3], data_list.data());

    SECTION("construction") {
        SECTION("default construction") {
            bc::list_bview bv{};
            CHECK(holds_uninitialized(bv));
        }
        SECTION("construction from pointers") {
            bc::list_bview bv(begin(descriptors_list), data_list.data());
            CHECK(holds_list(bv));
        }
        SECTION("converting constructor") {
            bc::list_bview bv(l_view_const);
            CHECK(holds_list(bv));
        }
    }
    SECTION("assignment") {
        bc::list_bview bv = list;
        CHECK(holds_list(bv));
    }

    SECTION("equality comparison") {
        bc::list_bview bv = list;
        CHECK(bv == list);
    }
    SECTION("order comparison") {
        bc::list_bview bv = list;
        CHECK_FALSE(bv < list);
        CHECK_FALSE(bv > list);
        CHECK(bv >= list);
        CHECK(bv <= list);
    }

    SECTION("at()") {
        auto ref = list.at(0);
        CHECK(ref == 2);
        CHECK_THROWS_AS(list.at(3), bc::out_of_range);
    }

    SECTION("front()") {
        CHECK(list.front() == 2);
        CHECK(list_reverse.front() == 2);
    }
    SECTION("back()") {
        CHECK(list.back() == 3);
        CHECK(list_reverse.back() == 3);
    }

    SECTION("empty()") {
        CHECK_FALSE(list.empty());
    }

    SECTION("size()"){
        CHECK(list.size() == 2);
    }
    SECTION("max_size()") {
        CHECK(list.max_size() == std::numeric_limits<std::uint32_t>::max());
    }
    SECTION("max_size()") {
        CHECK(list.max_size() == std::numeric_limits<std::uint32_t>::max());
    }

    SECTION("iterators") {
        SECTION("simple") {
            auto first = list.begin();
            ++first;
            ++first; // points to list end
            CHECK(first == list.end());
            --first;
            --first; // point to list begin
            CHECK(first == list.begin());
            CHECK(std::distance(list.begin(), list.end()) == list.size());
            CHECK(std::distance(list.rbegin(), list.rend()) == list.size());
            CHECK(std::distance(list.cbegin(), list.cend()) == list.size());
            CHECK(std::distance(list.crbegin(), list.crend()) == list.size());
        }

        SECTION("nested") {
            auto first = nested_list.begin();
            ++first; // points to first key
            ++first; // points to end
            CHECK(first == nested_list.end());
            --first;
            --first;
            CHECK(first == nested_list.begin());
            CHECK(std::distance(nested_list.begin(), nested_list.end()) == nested_list.size());
            CHECK(std::distance(nested_list.crbegin(), nested_list.crend()) == nested_list.size());

        }
        SECTION("iterators when descriptor is end descriptor") {
            auto first = list_reverse.begin();
            auto last = list_reverse.end();
            CHECK(first == list.begin());
            CHECK(last == list.end());
        }
    }

    SECTION("bencoded_view") {
        CHECK(list.bencoded_view() == data_list);
    }

    SECTION("comparison with bview") {
        CHECK(l_view == list);
        CHECK_FALSE(l_view != list);
        CHECK(l_view <= list);
        CHECK(l_view >= list);
    }

    SECTION("comparison with other bview types")
    {
        CHECK(list > i_view);
        CHECK(list > s_view);
        CHECK(list < d_view);
    }
}