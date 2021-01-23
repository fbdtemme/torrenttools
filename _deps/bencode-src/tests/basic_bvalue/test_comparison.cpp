////
//// Created by fbdtemme on 02/06/19.
////
#include <vector>
#include <string>
#include <string_view>

#include <catch2/catch.hpp>

#include "bencode/traits/all.hpp"
#include "bencode/bvalue.hpp"


using namespace bencode;
using namespace std::string_literals;
using namespace std::string_view_literals;


TEST_CASE("test equality", "[bvalue]")
{
    const std::array<bencode::bvalue, 10> b_ref {
            -17, 42, "foo"sv, "bar"sv, "foo", "bar",
            bencode::bvalue(btype::list, {1, 2, 3}),
            bencode::bvalue(btype::list, {"one", "two", "three"}),
            {{"first", 1}, {"second", 2}},
            {{"a", "a_"}, {"b", {"B"}}},
    };

    SECTION("integer") {
        bencode::bvalue b_value{42};

        auto expected_values = std::array {
            false, true, false, false, false, false, false, false, false, false
        };

        for (std::size_t i = 0; i < std::size(b_ref) ; ++i) {
            auto& input = b_ref[i];
            auto& expected = expected_values[i];

            CHECK((b_value == input) == expected);
            CHECK((b_value != input) == !expected);
        }
    }
//
//    SECTION("string_view") {
//        bencode::bvalue b_value{"bar"sv};
//
//        auto expected_values = std::array{
//            false, false, false, true, false, true, false, false, false, false
//        };
//
//        for (int i = 0; i < std::size(b_ref) ; ++i) {
//            auto& input = b_ref[i];
//            auto& expected = expected_values[i];
//
//            CHECK((b_value == input) == expected);
//            CHECK((b_value != input) == !expected);
//        }
//    }

    SECTION("string") {
        bencode::bvalue b_value{"foo"};

        auto expected_values = std::array{
            false, false, true, false, true, false, false, false, false, false
        };

        for (std::size_t i = 0; i < std::size(b_ref) ; ++i) {
            auto& input = b_ref[i];
            auto& expected = expected_values[i];

            CHECK((b_value == input) == expected);
            CHECK((b_value != input) == !expected);
        }
    }

    SECTION("list") {
        auto b_value = bencode::bvalue(btype::list, {1, 2, 3});

        auto expected_values = std::array{
            false, false, false, false, false, false, true, false, false, false
        };

        for (std::size_t i = 0; i < std::size(b_ref) ; ++i) {
            auto& input = b_ref[i];
            auto& expected = expected_values[i];

            CHECK((b_value == input) == expected);
            CHECK((b_value != input) == !expected);
        }
    }

    SECTION("dict") {
        auto b_value = bencode::bvalue(btype::dict, {{"first", 1}, {"second", 2}});

        auto expected_values = std::array {
            false, false, false, false, false, false, false, false, true, false
        };

        for (std::size_t i = 0; i < std::size(b_ref) ; ++i) {
            auto& input = b_ref[i];
            auto& expected = expected_values[i];

            CHECK((b_value == input) == expected);
            CHECK((b_value != input) == !expected);
        }
    }
}



TEST_CASE("test comparison operators", "[bvalue]")
{
    bencode::bvalue i_ref = 4;
    bencode::bvalue sv_ref = "foo"sv;
    bencode::bvalue s_ref = "foo"s;
    bencode::bvalue l_ref = bencode::bvalue(btype::list, {1, 2, 3});
    bencode::bvalue d_ref {{"first", 1}, {"second", 2}};

    SECTION("integer") {
        // implicit conversion to of long to bencode::bvalue
        bencode::bvalue v = 4;

        SECTION("compare with integer") {
            auto c_greater = 10;
            auto c_equal = 4;
            auto c_less = -8;

            CHECK(v < c_greater);
            CHECK(c_greater > v);
            CHECK(v <= c_greater);
            CHECK(c_greater >= v);
            CHECK(v <= c_equal);
            CHECK(c_equal >= v);
            CHECK(v == c_equal);
            CHECK(c_equal == v);
            CHECK(v >= c_equal);
            CHECK(c_equal <= v);
            CHECK(v >= c_less);
            CHECK(c_less <= v);
            CHECK(v > c_less);
            CHECK(c_less < v);
        }

        SECTION("compare with other types") {
            auto o_greater = GENERATE_COPY(values({sv_ref, s_ref, l_ref, d_ref}));
            CHECK(v < o_greater);
            CHECK(v <= o_greater);
            CHECK_FALSE(v == o_greater);
            CHECK_FALSE(v >= o_greater);
            CHECK_FALSE(v > o_greater);
        }
    }

    SECTION("string_view") {
        bencode::bvalue v = "foo"sv;

        SECTION("compare with string_view") {
            auto c_greater = "zzz"sv;
            auto c_equal = v;
            auto c_less = "aaa"sv;

            CHECK(v < c_greater);       CHECK(c_greater > v);
            CHECK(v <= c_greater);      CHECK(c_greater >= v);
            CHECK(v <= c_equal);        CHECK(c_equal >= v);
            CHECK(v == c_equal);        CHECK(c_equal == v);
            CHECK(v >= c_equal);        CHECK(c_equal <= v);
            CHECK(v >= c_less);         CHECK(c_less <= v);
            CHECK(v > c_less);          CHECK(c_less < v);
        }

        SECTION("compare with string") {
            auto c_greater = "zzz"s;
            auto c_equal = "foo"s;
            auto c_less = "aaa"s;

            CHECK(v < c_greater);       CHECK(c_greater > v);
            CHECK(v <= c_greater);      CHECK(c_greater >= v);
            CHECK(v <= c_equal);        CHECK(c_equal >= v);
            CHECK(v == c_equal);        CHECK(c_equal == v);
            CHECK(v >= c_equal);        CHECK(c_equal <= v);
            CHECK(v >= c_less);         CHECK(c_less <= v);
            CHECK(v > c_less);          CHECK(c_less < v);
        }

        SECTION("compare with other types") {
            SECTION("less") {
                auto o_less = GENERATE_REF(values({i_ref}));

                CHECK_FALSE(v < o_less);
                CHECK_FALSE(v <= o_less);
                CHECK_FALSE(v == o_less);
                CHECK(v >= o_less);
                CHECK(v > o_less);
            }

            SECTION("greater") {
                auto o_greater = GENERATE_REF(values({l_ref, d_ref}));
                CHECK(v < o_greater);
                CHECK(v <= o_greater);
                CHECK_FALSE(v == o_greater);
                CHECK_FALSE(v >= o_greater);
                CHECK_FALSE(v > o_greater);
            }
        }
    }

    SECTION("string") {
        bencode::bvalue v = "foo"s;

        SECTION("compare with string") {
            auto c_greater = "zzz"s;
            auto c_equal = "foo"s;
            auto c_less = "aaa"s;

            CHECK(v < c_greater);       CHECK(c_greater > v);
            CHECK(v <= c_greater);      CHECK(c_greater >= v);
            CHECK(v <= c_equal);        CHECK(c_equal >= v);
            CHECK(v == c_equal);        CHECK(c_equal == v);
            CHECK(v >= c_equal);        CHECK(c_equal <= v);
            CHECK(v >= c_less);         CHECK(c_less <= v);
            CHECK(v > c_less);          CHECK(c_less < v);
        }

        SECTION("compare with string_view") {
            auto c_greater = "zzz"sv;
            auto c_equal = v;
            auto c_less = "aaa"sv;

            CHECK(v < c_greater);       CHECK(c_greater > v);
            CHECK(v <= c_greater);      CHECK(c_greater >= v);
            CHECK(v <= c_equal);        CHECK(c_equal >= v);
            CHECK(v == c_equal);        CHECK(c_equal == v);
            CHECK(v >= c_equal);        CHECK(c_equal <= v);
            CHECK(v >= c_less);         CHECK(c_less <= v);
            CHECK(v > c_less);          CHECK(c_less < v);
        }

        SECTION("compare with other types") {
            SECTION("less") {
                auto o_less = GENERATE_REF(values({i_ref}));

                CHECK_FALSE(v < o_less);
                CHECK_FALSE(v <= o_less);
                CHECK_FALSE(v == o_less);
                CHECK(v >= o_less);
                CHECK(v > o_less);
            }

            SECTION("greater") {
                auto o_greater = GENERATE_REF(values({l_ref, d_ref}));
                CHECK(v < o_greater);
                CHECK(v <= o_greater);
                CHECK_FALSE(v == o_greater);
                CHECK_FALSE(v >= o_greater);
                CHECK_FALSE(v > o_greater);
            }
        }
    }

    SECTION("list") {
        auto v = bencode::bvalue(btype::list, {1, 2, 3});

        SECTION("compare with list") {
            auto c_greater = bencode::bvalue(btype::list, {"one", "two", "three"});
            auto c_equal = v;
            auto c_less = bencode::bvalue(btype::list, {0, 2});

            CHECK(v < c_greater);       CHECK(c_greater > v);
            CHECK(v <= c_greater);      CHECK(c_greater >= v);
            CHECK(v <= c_equal);        CHECK(c_equal >= v);
            CHECK(v == c_equal);        CHECK(c_equal == v);
            CHECK(v >= c_equal);        CHECK(c_equal <= v);
            CHECK(v >= c_less);         CHECK(c_less <= v);
            CHECK(v > c_less);          CHECK(c_less < v);
        }

        SECTION("compare with other types") {
            SECTION("less") {
                auto o_less = GENERATE_REF(values({i_ref, sv_ref, s_ref}));

                CHECK_FALSE(v < o_less);
                CHECK_FALSE(v <= o_less);
                CHECK_FALSE(v == o_less);
                CHECK(v >= o_less);
                CHECK(v > o_less);
            }

            SECTION("greater") {
                auto o_greater = GENERATE_REF(values({d_ref}));
                CHECK(v < o_greater);
                CHECK(v <= o_greater);
                CHECK_FALSE(v == o_greater);
                CHECK_FALSE(v >= o_greater);
                CHECK_FALSE(v > o_greater);
            }
        }
    }


    SECTION("dict") {
        auto v = bencode::bvalue(btype::dict, {{"foo", 1}, {"bar", 2}});

        SECTION("compare with list") {
            auto c_greater = bencode::bvalue(btype::dict, {{"foo", 2}, {"second", 2}});
            auto c_equal = v;
            auto c_less = bencode::bvalue(btype::dict, {{"a", "a_"}, {"b", {{"B", 1}}}});

            CHECK(v < c_greater);       CHECK(c_greater > v);
            CHECK(v <= c_greater);      CHECK(c_greater >= v);
            CHECK(v <= c_equal);        CHECK(c_equal >= v);
            CHECK(v == c_equal);        CHECK(c_equal == v);
            CHECK(v >= c_equal);        CHECK(c_equal <= v);
            CHECK(v >= c_less);         CHECK(c_less <= v);
            CHECK(v > c_less);          CHECK(c_less < v);
        }

        SECTION("compare with other types") {
           auto o_less = GENERATE_REF(values({i_ref, sv_ref, s_ref, l_ref}));

            CHECK_FALSE(v < o_less);
            CHECK_FALSE(v <= o_less);
            CHECK_FALSE(v == o_less);
            CHECK(v >= o_less);
            CHECK(v > o_less);
        }
    }
}



TEST_CASE("test direct comparison operators", "[bvalue]")
{
    bencode::bvalue i_ref = 4;
    bencode::bvalue sv_ref = "foo"sv;
    bencode::bvalue s_ref = "foo"s;
    bencode::bvalue l_ref = bencode::bvalue(btype::list, {1, 2, 3});
    bencode::bvalue d_ref {{"first", 1}, {"second", 2}};

    SECTION("integer") {
        // implicit conversion to of long to bencode::bvalue
        bencode::bvalue v = 4;

        SECTION("compare with integer") {
            auto c_greater = 10;
            auto c_equal = 4;
            auto c_less = -8;

            CHECK(v < c_greater);       CHECK(c_greater > v);
            CHECK(v <= c_greater);      CHECK(c_greater >= v);
            CHECK(v <= c_equal);        CHECK(c_equal >= v);
            CHECK(v == c_equal);        CHECK(c_equal == v);
            CHECK(v >= c_equal);        CHECK(c_equal <= v);
            CHECK(v >= c_less);         CHECK(c_less <= v);
            CHECK(v > c_less);          CHECK(c_less < v);
        }

        SECTION("compare with other types") {
            auto o_greater = GENERATE_REF(values({sv_ref, s_ref, l_ref, d_ref}));
            CHECK(v < o_greater);
            CHECK(v <= o_greater);
            CHECK_FALSE(v == o_greater);
            CHECK_FALSE(v >= o_greater);
            CHECK_FALSE(v > o_greater);
        }
    }

    SECTION("string_view") {
        bencode::bvalue v = "foo"sv;

        SECTION("compare with string_view") {
            auto c_greater = "zzz"sv;
            auto c_equal = v;
            auto c_less = "aaa"sv;

            CHECK(v < c_greater);       CHECK(c_greater > v);
            CHECK(v <= c_greater);      CHECK(c_greater >= v);
            CHECK(v <= c_equal);        CHECK(c_equal >= v);
            CHECK(v == c_equal);        CHECK(c_equal == v);
            CHECK(v >= c_equal);        CHECK(c_equal <= v);
            CHECK(v >= c_less);         CHECK(c_less <= v);
            CHECK(v > c_less);          CHECK(c_less < v);
        }

        SECTION("compare with string") {
            auto c_greater = "zzz"s;
            auto c_equal = "foo"s;
            auto c_less = "aaa"s;

            CHECK(v < c_greater);       CHECK(c_greater > v);
            CHECK(v <= c_greater);      CHECK(c_greater >= v);
            CHECK(v <= c_equal);        CHECK(c_equal >= v);
            CHECK(v == c_equal);        CHECK(c_equal == v);
            CHECK(v >= c_equal);        CHECK(c_equal <= v);
            CHECK(v >= c_less);         CHECK(c_less <= v);
            CHECK(v > c_less);          CHECK(c_less < v);

//            auto r = v <=> c_greater;
        }

        SECTION("compare with other types") {
            SECTION("less") {
                auto o_less = GENERATE_REF(values({i_ref}));

                CHECK_FALSE(v < o_less);
                CHECK_FALSE(v <= o_less);
                CHECK_FALSE(v == o_less);
                CHECK(v >= o_less);
                CHECK(v > o_less);
            }

            SECTION("greater") {
                auto o_greater = GENERATE_REF(values({l_ref, d_ref}));
                CHECK(v < o_greater);
                CHECK(v <= o_greater);
                CHECK_FALSE(v == o_greater);
                CHECK_FALSE(v >= o_greater);
                CHECK_FALSE(v > o_greater);
            }
        }
    }

    SECTION("string") {
        bencode::bvalue v = "foo"s;

        SECTION("compare with string") {
            auto c_greater = "zzz"s;
            auto c_equal = "foo"s;
            auto c_less = "aaa"s;

            CHECK(v < c_greater);       CHECK(c_greater > v);
            CHECK(v <= c_greater);      CHECK(c_greater >= v);
            CHECK(v <= c_equal);        CHECK(c_equal >= v);
            CHECK(v == c_equal);        CHECK(c_equal == v);
            CHECK(v >= c_equal);        CHECK(c_equal <= v);
            CHECK(v >= c_less);         CHECK(c_less <= v);
            CHECK(v > c_less);          CHECK(c_less < v);
        }

        SECTION("compare with string_view") {
            auto c_greater = "zzz"sv;
            auto c_equal = v;
            auto c_less = "aaa"sv;

            CHECK(v < c_greater);       CHECK(c_greater > v);
            CHECK(v <= c_greater);      CHECK(c_greater >= v);
            CHECK(v <= c_equal);        CHECK(c_equal >= v);
            CHECK(v == c_equal);        CHECK(c_equal == v);
            CHECK(v >= c_equal);        CHECK(c_equal <= v);
            CHECK(v >= c_less);         CHECK(c_less <= v);
            CHECK(v > c_less);          CHECK(c_less < v);
        }

        SECTION("compare with other types") {
            SECTION("less") {
                auto o_less = GENERATE_REF(values({i_ref}));

                CHECK_FALSE(v < o_less);
                CHECK_FALSE(v <= o_less);
                CHECK_FALSE(v == o_less);
                CHECK(v >= o_less);
                CHECK(v > o_less);
            }

            SECTION("greater") {
                auto o_greater = GENERATE_REF(values({l_ref, d_ref}));
                CHECK(v < o_greater);
                CHECK(v <= o_greater);
                CHECK_FALSE(v == o_greater);
                CHECK_FALSE(v >= o_greater);
                CHECK_FALSE(v > o_greater);
            }
        }
    }

    SECTION("list") {
        auto v = bencode::bvalue(btype::list, {1, 2, 3});

        SECTION("compare with list") {
            auto c_greater = bencode::bvalue(btype::list, {"one", "two", "three"});
            auto c_equal = v;
            auto c_less = bencode::bvalue(btype::list, {0, 2});

            CHECK(v < c_greater);       CHECK(c_greater > v);
            CHECK(v <= c_greater);      CHECK(c_greater >= v);
            CHECK(v <= c_equal);        CHECK(c_equal >= v);
            CHECK(v == c_equal);        CHECK(c_equal == v);
            CHECK(v >= c_equal);        CHECK(c_equal <= v);
            CHECK(v >= c_less);         CHECK(c_less <= v);
            CHECK(v > c_less);          CHECK(c_less < v);
        }

        SECTION("compare with other types") {
            SECTION("less") {
                auto o_less = GENERATE_REF(values({i_ref, sv_ref, s_ref}));

                CHECK_FALSE(v < o_less);
                CHECK_FALSE(v <= o_less);
                CHECK_FALSE(v == o_less);
                CHECK(v >= o_less);
                CHECK(v > o_less);
            }

            SECTION("greater") {
                auto o_greater = GENERATE_REF(values({d_ref}));
                CHECK(v < o_greater);
                CHECK(v <= o_greater);
                CHECK_FALSE(v == o_greater);
                CHECK_FALSE(v >= o_greater);
                CHECK_FALSE(v > o_greater);
            }
        }
    }


    SECTION("dict") {
        auto v = bencode::bvalue(btype::dict, {{"foo", 1}, {"bar", 2}});

        SECTION("compare with list") {
            auto c_greater = bencode::bvalue(btype::dict, {{"foo", 2}, {"second", 2}});
            auto c_equal = v;
            auto c_less = bencode::bvalue(btype::dict, {{"a", "a_"}, {"b", {{"B", 1}}}});

            CHECK(v < c_greater);       CHECK(c_greater > v);
            CHECK(v <= c_greater);      CHECK(c_greater >= v);
            CHECK(v <= c_equal);        CHECK(c_equal >= v);
            CHECK(v == c_equal);        CHECK(c_equal == v);
            CHECK(v >= c_equal);        CHECK(c_equal <= v);
            CHECK(v >= c_less);         CHECK(c_less <= v);
            CHECK(v > c_less);          CHECK(c_less < v);
        }

        SECTION("compare with other types") {
            auto o_less = GENERATE_REF(values({i_ref, sv_ref, s_ref, l_ref}));

            CHECK_FALSE(v < o_less);
            CHECK_FALSE(v <= o_less);
            CHECK_FALSE(v == o_less);
            CHECK(v >= o_less);
            CHECK(v > o_less);
        }
    }

    SECTION("std::vector<std::byte>")
    {
        auto bv = std::vector<std::byte>{std::byte('a'), std::byte('b'), std::byte('c')};
        auto v = bencode::bvalue(btype::string, "abc");

        CHECK(bv == v);
        CHECK(bv <= v);
        CHECK(bv >= v);
        CHECK_FALSE(bv < v);
        CHECK_FALSE(bv > v);
    }
}
