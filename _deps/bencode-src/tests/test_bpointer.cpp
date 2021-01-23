//
// Created by fbdtemme on 9/2/20.
//

#include <array>
#include <catch2/catch.hpp>
#include "bencode/bvalue.hpp"
#include "bencode/bview.hpp"
#include "bencode/detail/bpointer.hpp"


using namespace bencode;
using namespace bencode::literals;

static const auto b_value = bvalue{
        {"foo", bvalue(btype::list, {"bar", "baz"})},
        {"", 0},
        {"a/b", 1},
        {"c%d", 2},
        {"e^f", 3},
        {"g|h", 4},
        {"i\\j", 5},
        {"k\"l", 6},
        {" ", 7},
        {"m~n", 8}
};


static const auto b_encoded = bencode::encode(b_value);
static const auto b_desc_table = bencode::decode_view(b_encoded);
static const auto b_view = b_desc_table.get_root();

TEST_CASE("test bpointer")
{
    SECTION("construction") {
        SECTION("default construction") {
            auto p = bpointer();
            CHECK(p.empty());
        }
        SECTION("initialized_list construction") {
            auto p = bpointer({"foo", "bar"});
            CHECK(p == "/foo/bar");
        }
        SECTION("error - no slash") {
            CHECK_THROWS_AS(bpointer("foo"), bpointer_error);
            CHECK_THROWS_WITH(bpointer("foo"),
                    "bencode pointer must be empty or begin with '/'");
        }
        SECTION("error - bad escaping") {
            CHECK_THROWS_AS(bpointer("/~~"), bpointer_error);
            CHECK_THROWS_AS(bpointer("/~"), bpointer_error);
        }
    }

    SECTION("empty()"){
        auto p1 = bpointer();
        CHECK(p1.empty());
        auto p2 = bpointer("/foo");
        CHECK_FALSE(p2.empty());
    }

    SECTION("appending")
    {
        auto foo = bpointer("/foo");
        auto foo_bar = "/foo/bar"_bpointer;
        auto foo_1 = "/foo/1"_bpointer;
        auto bar_baz = "/bar/baz"_bpointer;
        auto foo_bar_baz = "/foo/bar/baz"_bpointer;

        SECTION("string - append") {
            foo.append("bar");
            CHECK(foo == foo_bar);
        }
        SECTION("string - operator/=") {
            foo /= "bar";
            CHECK(foo == foo_bar);
        }
        SECTION("int - append") {
            foo.append(1);
            CHECK(foo == foo_1);
        }
        SECTION("int - operator/=") {
            foo /= 1;
            CHECK(foo == foo_1);
        }
        SECTION("bpointer - append") {
            foo.append(bar_baz);
            CHECK(foo == foo_bar_baz);
        }
        SECTION("operator/")
        {
            CHECK("/foo"_bpointer / "bar" == foo_bar);
            CHECK("/foo"_bpointer / 1 == foo_1);
            CHECK("/foo"_bpointer / "/bar/baz"_bpointer == foo_bar_baz);
        }
    }

    SECTION("iteration")
    {
        auto foo_bar_baz = "/foo/bar/baz"_bpointer;
        const auto const_foo_bar_baz = "/foo/bar/baz"_bpointer;

        std::array<std::string_view, 3> check{"foo", "bar", "baz"};

        SECTION("non const") {
            CHECK(std::equal(foo_bar_baz.begin(), foo_bar_baz.end(),
                    check.begin(), check.end()));
            CHECK(std::equal(foo_bar_baz.cbegin(), foo_bar_baz.cend(),
                    check.begin(), check.end()));
        }
        SECTION("const") {
            CHECK(std::equal(const_foo_bar_baz.begin(), const_foo_bar_baz.end(),
                    check.begin(), check.end()));
            CHECK(std::equal(const_foo_bar_baz.cbegin(), const_foo_bar_baz.cend(),
                             check.begin(), check.end()));
        }
        SECTION("reverse non const") {
            CHECK(std::equal(foo_bar_baz.rbegin(), foo_bar_baz.rend(),
                             check.rbegin(), check.rend()));
            CHECK(std::equal(foo_bar_baz.crbegin(), foo_bar_baz.crend(),
                    check.rbegin(), check.rend()));
        }
        SECTION("reverse const") {
            CHECK(std::equal(const_foo_bar_baz.rbegin(), const_foo_bar_baz.rend(),
                    check.rbegin(), check.rend()));
            CHECK(std::equal(const_foo_bar_baz.crbegin(), const_foo_bar_baz.crend(),
                    check.rbegin(), check.rend()));
        }
    }

    SECTION("front/back")
    {
        auto foo_bar = bpointer("/foo/bar");
        const auto const_foo_bar = bpointer("/foo/bar");

        CHECK(foo_bar.front() == "foo");
        CHECK(foo_bar.back() == "bar");
        CHECK(const_foo_bar.front() == "foo");
        CHECK(const_foo_bar.back() == "bar");
    }

    SECTION("equality comparison")
    {
        auto foo_bar = bpointer("/foo/bar");

        CHECK(foo_bar == foo_bar);
        CHECK_FALSE(foo_bar != foo_bar);
        CHECK(foo_bar == "/foo/bar");
        CHECK_FALSE(foo_bar != "/foo/bar");

        CHECK(bpointer("/i~0j/~12") == "/i~0j/~12");
    }

    SECTION("order comparsion")
    {
        auto foo_bar = "/foo/bar"_bpointer;
        auto bar_baz = "/bar/baz"_bpointer;

        CHECK(bar_baz < foo_bar);
        CHECK(bar_baz <= foo_bar);
        CHECK(foo_bar > bar_baz);
        CHECK(foo_bar >= bar_baz);
    }


    SECTION("examples from RFC 6901") {
        SECTION("const access - bvalue") {
            const auto& b = b_value;
            const auto b_dict = get_dict(b);
            // the whole document
            CHECK(b.at(bpointer()) == b);
            CHECK(b.at(bpointer("")) == b);

            // checked array access
            CHECK(b.at(bpointer("/foo"))   == b_dict.at("foo"));
            CHECK(b.at(bpointer("/foo/0")) == b_dict.at("foo").at(0));
            CHECK(b.at(bpointer("/foo/1")) == b_dict.at("foo")[1]);

            // empty string access
            CHECK(b.at(bpointer("/"))      == b_dict.at(""));

            // other cases
            CHECK(b.at(bpointer("/ ")) == b_dict.at(" "));
            CHECK(b.at(bpointer("/c%d")) == b_dict.at("c%d"));
            CHECK(b.at(bpointer("/e^f")) == b_dict.at("e^f"));
            CHECK(b.at(bpointer("/g|h")) == b_dict.at("g|h"));
            CHECK(b.at(bpointer("/i\\j")) == b_dict.at("i\\j"));
            CHECK(b.at(bpointer("/k\"l")) == b_dict.at("k\"l"));

            // escaped access
            auto p1 = bpointer("/a~1b");
            CHECK(b.at(p1) == b_dict.at("a/b"));
            auto p2 = bpointer("/m~0n");
            CHECK(b.at(p2) == b_dict.at("m~n"));

            // unresolved access
            bvalue b_primitive = 1;
            CHECK_THROWS_AS(b_primitive.at("/foo"_bpointer), out_of_range);
        }

        SECTION("non const access - bvalue") {
            auto& b = const_cast<bvalue&>(b_value);

            // the whole document
            CHECK(b.at(bpointer()) == b);
            CHECK(b.at(bpointer("")) == b);

            // checked array access
            CHECK(b.at(bpointer("/foo"))   == b.at("foo"));
            CHECK(b.at(bpointer("/foo/0")) == b.at("foo")[0]);
            CHECK(b.at(bpointer("/foo/1")) == b.at("foo")[1]);

            // empty string access
            CHECK(b.at(bpointer("/"))      == b.at(""));

            // other cases
            CHECK(b.at(bpointer("/ ")) == b.at(" "));
            CHECK(b.at(bpointer("/c%d")) == b.at("c%d"));
            CHECK(b.at(bpointer("/e^f")) == b.at("e^f"));
            CHECK(b.at(bpointer("/g|h")) == b.at("g|h"));
            CHECK(b.at(bpointer("/i\\j")) == b.at("i\\j"));
            CHECK(b.at(bpointer("/k\"l")) == b.at("k\"l"));

            // escaped access
            auto p1 = bpointer("/a~1b");
            CHECK(b.at(p1) == b.at("a/b"));
            auto p2 = bpointer("/m~0n");
            CHECK(b.at(p2) == b.at("m~n"));

            // unresolved access
            bvalue b_primitive = 1;
            CHECK_THROWS_AS(b_primitive.at("/foo"_bpointer), out_of_range);
        }

        SECTION("const access - bview")
        {
            const auto& b = b_view;

            // the whole document
            CHECK(b.at(bpointer()) == b);
            CHECK(b.at(bpointer("")) == b);

            // checked array access
            CHECK(b.at(bpointer("/foo"))   == b.at("foo"));
            CHECK(b.at(bpointer("/foo/0")) == b.at("foo")[0]);
            CHECK(b.at(bpointer("/foo/1")) == b.at("foo")[1]);

            // empty string access
            CHECK(b.at(bpointer("/"))      == b.at(""));

            // other cases
            CHECK(b.at(bpointer("/ ")) == b.at(" "));
            CHECK(b.at(bpointer("/c%d")) == b.at("c%d"));
            CHECK(b.at(bpointer("/e^f")) == b.at("e^f"));
            CHECK(b.at(bpointer("/g|h")) == b.at("g|h"));
            CHECK(b.at(bpointer("/i\\j")) == b.at("i\\j"));
            CHECK(b.at(bpointer("/k\"l")) == b.at("k\"l"));

            // escaped access
            auto p1 = bpointer("/a~1b");
            CHECK(b.at(p1) == b.at("a/b"));
            auto p2 = bpointer("/m~0n");
            CHECK(b.at(p2) == b.at("m~n"));

            // unresolved access
            bview b_primitive {};
            CHECK_THROWS_AS(b_primitive.at("/foo"_bpointer), out_of_range);
        }
        SECTION("non const access - bview") {
            auto& b = b_view;

            // the whole document
            CHECK(b.at(bpointer()) == b);
            CHECK(b.at(bpointer("")) == b);

            // checked array access
            CHECK(b.at(bpointer("/foo"))   == b.at("foo"));
            CHECK(b.at(bpointer("/foo/0")) == b.at("foo")[0]);
            CHECK(b.at(bpointer("/foo/1")) == b.at("foo")[1]);

            // empty string access
            CHECK(b.at(bpointer("/"))      == b.at(""));

            // other cases
            CHECK(b.at(bpointer("/ ")) == b.at(" "));
            CHECK(b.at(bpointer("/c%d")) == b.at("c%d"));
            CHECK(b.at(bpointer("/e^f")) == b.at("e^f"));
            CHECK(b.at(bpointer("/g|h")) == b.at("g|h"));
            CHECK(b.at(bpointer("/i\\j")) == b.at("i\\j"));
            CHECK(b.at(bpointer("/k\"l")) == b.at("k\"l"));

            // escaped access
            auto p1 = bpointer("/a~1b");
            CHECK(b.at(p1) == b.at("a/b"));
            auto p2 = bpointer("/m~0n");
            CHECK(b.at(p2) == b.at("m~n"));

            // unresolved access
            bview b_primitive {};
            CHECK_THROWS_AS(b_primitive.at("/foo"_bpointer), out_of_range);
        }

        SECTION("contains - bview")
        {
            const auto& b = b_view;

            // the whole document
            CHECK(b.contains(bpointer()));
            CHECK(b.contains(bpointer("")));

            // checked array access
            CHECK(b.contains(bpointer("/foo")));
            CHECK(b.contains(bpointer("/foo/0")));
            CHECK(b.contains(bpointer("/foo/1")));

            // empty string access
            CHECK(b.contains(bpointer("/")));

            // other cases
            CHECK(b.contains(bpointer("/ ")));
            CHECK(b.contains(bpointer("/c%d")));
            CHECK(b.contains(bpointer("/e^f")));
            CHECK(b.contains(bpointer("/g|h")));
            CHECK(b.contains(bpointer("/i\\j")));
            CHECK(b.contains(bpointer("/k\"l")));

            // escaped access
            auto p1 = bpointer("/a~1b");
            CHECK(b.contains(p1));
            auto p2 = bpointer("/m~0n");
            CHECK(b.contains(p2));

            // unresolved access
            bview b_primitive {};
            CHECK_FALSE(b_primitive.contains("/foo"_bpointer));
        }


        SECTION("contains - bvalue")
        {
            const auto& b = b_value;

            // the whole document
            CHECK(b.contains(bpointer()));
            CHECK(b.contains(bpointer("")));

            // checked array access
            CHECK(b.contains(bpointer("/foo")));
            CHECK(b.contains(bpointer("/foo/0")));
            CHECK(b.contains(bpointer("/foo/1")));

            // empty string access
            CHECK(b.contains(bpointer("/")));

            // other cases
            CHECK(b.contains(bpointer("/ ")));
            CHECK(b.contains(bpointer("/c%d")));
            CHECK(b.contains(bpointer("/e^f")));
            CHECK(b.contains(bpointer("/g|h")));
            CHECK(b.contains(bpointer("/i\\j")));
            CHECK(b.contains(bpointer("/k\"l")));

            // escaped access
            auto p1 = bpointer("/a~1b");
            CHECK(b.contains(p1));
            auto p2 = bpointer("/m~0n");
            CHECK(b.contains(p2));

            // unresolved access
            bview b_primitive {};
            CHECK_FALSE(b_primitive.contains("/foo"_bpointer));
        }
    }
}