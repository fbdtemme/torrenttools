#include <catch2/catch.hpp>

#include "bencode/bview.hpp"

#include <type_traits>
#include <string>
#include <string_view>

#include "data.hpp"

using namespace std::string_literals;
namespace bc = bencode;


TEST_CASE("test dict_bview") {
    bc::dict_bview dict(begin(descriptors_dict), data_dict.data());
    bc::dict_bview nested_dict(begin(descriptors_nested_dict), data_nested_dict.data());

    SECTION("construction") {
        SECTION("default construction") {
            bc::dict_bview bv{};
            CHECK(holds_uninitialized(bv));
        }
        SECTION("construction from pointers") {
            bc::dict_bview bv(begin(descriptors_dict), data_dict.data());
            CHECK(holds_dict(bv));
        }
        SECTION("converting constructor") {
            bc::dict_bview bv(d_view_const);
            CHECK(holds_dict(bv));
        }
    }
    SECTION("assignment") {
        bc::dict_bview bv = dict;
        CHECK(holds_dict(bv));
    }

    SECTION("equality comparison") {
        bc::dict_bview bv = dict;
        CHECK(bv == dict);
    }
    SECTION("order comparison") {
        bc::dict_bview bv = dict;
        CHECK_FALSE(bv < dict);
        CHECK_FALSE(bv > dict);
        CHECK(bv >= dict);
        CHECK(bv <= dict);
    }

    SECTION("at()") {
        auto ref = dict.at("spam");
        CHECK(ref == 1);
        CHECK_THROWS_AS(dict.at("blah"), bc::out_of_range);
    }

    SECTION("empty()") {
        CHECK_FALSE(dict.empty());
    }

    SECTION("size()"){
        CHECK(dict.size() == 1);
    }
    SECTION("max_size()") {
        CHECK(dict.max_size() == std::numeric_limits<std::uint32_t>::max());
    }
    SECTION("max_size()") {
        CHECK(dict.max_size() == std::numeric_limits<std::uint32_t>::max());
    }
    SECTION("count()") {
        CHECK(dict.count("spam") == 1);
        CHECK(dict.count("bar") == 0);
    }
    SECTION("find()") {
        auto it = dict.find("spam");
        CHECK(it != dict.end());
        CHECK(it->first == "spam");
        CHECK(it->second == 1);
        CHECK(dict.find("bar") == dict.end());
    }
    SECTION("contains") {
        CHECK(dict.contains("spam"));
        CHECK_FALSE(dict.contains("bar"));
    }
    SECTION("equal_range") {
        SECTION("found") {
            auto[first, last] = dict.equal_range("spam");
            CHECK(first==dict.cbegin());
            CHECK(last==dict.cend());
        }
        SECTION("not found") {
            auto[first, last] = dict.equal_range("blah");
            CHECK(first==dict.cend());
            CHECK(last==dict.cend());
        }
    }
    SECTION("iterators") {
        SECTION("simple") {
            auto first = dict.begin();
            ++first; // points to dict end
            --first; // point to dict begin
            CHECK(first == dict.begin());
            CHECK(std::distance(dict.cbegin(), dict.cend()) == dict.size());
            CHECK(std::distance(dict.crbegin(), dict.crend()) == dict.size());
        }

        SECTION("nested") {
            auto first = nested_dict.begin();
            ++first; // points to second key
            ++first; // points to end
            CHECK(first == nested_dict.end());
            --first;
            --first;
            CHECK(first == nested_dict.begin());
            CHECK(std::distance(nested_dict.begin(), nested_dict.end()) == nested_dict.size());
            CHECK(std::distance(nested_dict.crbegin(), nested_dict.crend()) == nested_dict.size());

        }
        SECTION("iterators when descriptor is end descriptor") {
            auto bv = bc::dict_bview(&descriptors_dict[3], data_dict.data());
            auto first = bv.begin();
            auto last = bv.end();
            CHECK(first == dict.begin());
            CHECK(last == dict.end());
        }

    }

    SECTION("bencoded_view") {
        CHECK(dict.bencoded_view() == data_dict);
    }

    SECTION("comparison with dict_bview") {
        CHECK(d_view == dict);
        CHECK_FALSE(d_view != dict);
        CHECK(d_view <= dict);
        CHECK(d_view >= dict);
    }
    SECTION("comparison with other bview types")
    {
        CHECK(dict > i_view);
        CHECK(dict > s_view);
        CHECK(dict > l_view);
    }
}


//constexpr std::string_view example = (
//        "d"
//        "3:one"
//        "i1e"
//        "5:three"
//        "l"
//        "d"
//        "3:bar" "i0e" "3:foo" "i0e"
//        "e"
//        "e"
//        "3:two"
//        "l"
//        "i3e" "3:foo" "i4e"
//        "e"
//        "e"
//);
//
//template <typename Range>
//inline auto get_descriptors(Range& range) -> std::vector<bencode::descriptor>
//{
//    auto p = bencode::descriptor_parser();
//    auto r = p.parse(range);
//    if (!r) throw r.error();
//    return r->descriptors();
//}
//
//TEST_CASE("dict descriptor")
//{
//    auto descriptors = get_descriptors(example);
//    auto dict = bencode::dict_bview(descriptors.data(), example.data());
//
//    SECTION("begin()") {
//        auto it = std::begin(dict);
//        CHECK(get_integer(it->second) == 1);
//    }
//
//    SECTION("end()") {
//        auto end = std::end(dict);
//        SUCCEED();
//    }
//
//    SECTION("find") {
//        auto it = dict.find("one");
//        CHECK(it != dict.end());
//        CHECK(it->first == "one");
//        CHECK(get_integer(it->second) == 1);
//    }
//}