//
// Created by fbdtemme on 24/09/2019.
//
#include <catch2/catch.hpp>

#include "bencode/traits/all.hpp"
#include "bencode/bview.hpp"
#include "bencode/parsers/descriptor_parser.hpp"

#include <type_traits>
#include <string>
#include <string_view>

#include "data.hpp"


TEST_CASE("test get(bview)", "[bview][accessors]") {
    SECTION("const reference - access by enum/tag ") {
        const auto& i_ptr = get<bc::btype::integer>(i_view_const);
        const auto& s_ptr = get<bc::btype::string>(s_view_const);
        const auto& l_ptr = get<bc::btype::list>(l_view_const);
        const auto& m_ptr = get<bc::btype::dict>(d_view_const);
        SUCCEED();
    }

    SECTION("const reference - access by type ") {
        const auto& i_ptr = get<bc::integer_bview>(i_view_const);
        const auto& s_ptr = get<bc::string_bview>(s_view_const);
        const auto& l_ptr = get<bc::list_bview>(l_view_const);
        const auto& m_ptr = get<bc::dict_bview>(d_view_const);
        SUCCEED();
    }

    SECTION("reference - access by enum/tag ") {
        auto& i_ptr = get<bc::btype::integer>(i_view);
        auto& s_ptr = get<bc::btype::string>(s_view);
        auto& l_ptr = get<bc::btype::list>(l_view);
        auto& m_ptr = get<bc::btype::dict>(d_view);
        SUCCEED();
    }

    SECTION("reference - access by type ") {
        auto& i_ptr = get<bc::integer_bview>(i_view);
        auto& s_ptr = get<bc::string_bview>(s_view);
        auto& l_ptr = get<bc::list_bview>(l_view);
        auto& m_ptr = get<bc::dict_bview>(d_view);
        SUCCEED();
    }

    SECTION("throws when accessing with wrong type") {
        CHECK_THROWS_AS(get<bc::btype::list>(d_view_const), bc::bad_bview_access);
        CHECK_THROWS_AS(get<bc::btype::list>(d_view), bc::bad_bview_access);
        CHECK_THROWS_AS(get<bc::list_bview>(d_view_const), bc::bad_bview_access);
        CHECK_THROWS_AS(get<bc::list_bview>(d_view), bc::bad_bview_access);
    }
}


TEST_CASE("test get_if(descriptor)", "[bview][accessors]") {
    SECTION("const pointer - access by enum/tag ") {
        const auto* i_ptr = get_if<bc::btype::integer>(&i_view_const);
        const auto* s_ptr = get_if<bc::btype::string>(&s_view_const);
        const auto* l_ptr = get_if<bc::btype::list>(&l_view_const);
        const auto* m_ptr = get_if<bc::btype::dict>(&d_view_const);

        CHECK(i_ptr);
        CHECK(s_ptr);
        CHECK(l_ptr);
        CHECK(m_ptr);
    }

    SECTION("const pointer - access by type ") {
        const auto* i_ptr = get_if<bc::integer_bview>(&i_view_const);
        const auto* s_ptr = get_if<bc::string_bview>(&s_view_const);
        const auto* l_ptr = get_if<bc::list_bview>(&l_view_const);
        const auto* m_ptr = get_if<bc::dict_bview>(&d_view_const);

        CHECK(i_ptr);
        CHECK(s_ptr);
        CHECK(l_ptr);
        CHECK(m_ptr);
    }

    SECTION("pointer - access by enum/tag ") {
        auto* i_ptr = get_if<bc::btype::integer>(&i_view);
        auto* s_ptr = get_if<bc::btype::string>(&s_view);
        auto* l_ptr = get_if<bc::btype::list>(&l_view);
        auto* m_ptr = get_if<bc::btype::dict>(&d_view);

        CHECK(i_ptr);
        CHECK(s_ptr);
        CHECK(l_ptr);
        CHECK(m_ptr);
    }

    SECTION("pointer - access by type ") {
        auto* i_ptr = get_if<bc::integer_bview>(&i_view);
        auto* s_ptr = get_if<bc::string_bview>(&s_view);
        auto* l_ptr = get_if<bc::list_bview>(&l_view);
        auto* m_ptr = get_if<bc::dict_bview>(&d_view);

        CHECK(i_ptr);
        CHECK(s_ptr);
        CHECK(l_ptr);
        CHECK(m_ptr);
    }

    SECTION ("return nullptr when accessing with wrong type") {
        CHECK_FALSE(get_if<bc::btype::list>(&d_view_const));
        CHECK_FALSE(get_if<bc::btype::list>(&d_view));
        CHECK_FALSE(get_if<bc::list_bview>(&d_view_const));
        CHECK_FALSE(get_if<bc::list_bview>(&d_view));
    }
}


TEST_CASE("get_integer/get_if_integer", "[accessors]")
{
    SECTION("const reference") {
        const auto& i = get_integer(i_view_const);
        CHECK(i == 63);
    }
    SECTION("reference") {
        const auto& i = get_integer(i_view);
        CHECK(i == 63);
    }
    SECTION("const pointer") {
        const auto* i = get_if_integer(&i_view_const);
        CHECK(*i == 63);
    }

    SECTION("pointer") {
        auto* i = get_if_integer(&i_view);
        CHECK(*i == 63);
    }

    SECTION ("throws when accessing with wrong type") {
        REQUIRE_THROWS_AS(get_integer(s_view_const), bc::bad_bview_access);
        REQUIRE_THROWS_AS(get_integer(s_view), bc::bad_bview_access);
    }
}

TEST_CASE("get_string/get_if_string", "[accessors]")
{
    auto control = "spam";
    SECTION("const reference") {
        const auto& s = get_string(s_view_const);
        CHECK(s == control);
    }
    SECTION("reference") {
        auto& s = get_string(s_view);
        CHECK(s == control);
    }
    SECTION("const pointer") {
        const auto* s = get_if_string(&s_view_const);
        CHECK(*s == control);
    }
    SECTION("pointer") {
        auto* s = get_if_string(&s_view);
        CHECK(*s == control);
    }

    SECTION ("throws when accessing with wrong type") {
        REQUIRE_THROWS_AS(get_string(d_view_const), bc::bad_bview_access);
        REQUIRE_THROWS_AS(get_string(d_view), bc::bad_bview_access);
    }
}

TEST_CASE("get_list/get_if_list", "[accessors]")
{
    auto control = std::vector {2, 3};

    SECTION("const reference") {
        const auto& v = get_list(l_view_const);
        CHECK(v == control);
    }
    SECTION("reference") {
        auto& l = get_list(l_view);
        CHECK(l == control);
    }
    SECTION("const pointer") {
        const auto* l = get_if_list(&l_view_const);
        CHECK(*l == control);
    }
    SECTION("pointer") {
        auto* l = get_if_list(&l_view);
        CHECK(*l == control);
    }

    SECTION ("throws when accessing with wrong type") {
        REQUIRE_THROWS_AS(get_list(i_view_const), bc::bad_bview_access);
        REQUIRE_THROWS_AS(get_list(i_view), bc::bad_bview_access);
    }
}


TEST_CASE("get_dict/get_if_dict", "[accessors]")
{
    auto control = std::map<std::string, std::int64_t>{{"spam", 1}};

    SECTION("const reference") {
        const auto& i = get_dict(d_view_const);
        CHECK(i == control);
    }
    SECTION("reference") {
        auto& i = get_dict(d_view);
        CHECK(i == control);
    }

    SECTION("const pointer") {
        const auto* i = get_if_dict(&d_view_const);
        CHECK(*i == control);
    }

    SECTION("pointer") {
        auto* i = get_if_dict(&d_view);
        CHECK(*i == control);
    }

    SECTION ("throws when accessing with wrong type") {
        REQUIRE_THROWS_AS(get_dict(i_view_const), bc::bad_bview_access);
        REQUIRE_THROWS_AS(get_dict(i_view), bc::bad_bview_access);
    }
}

TEST_CASE("test current alternative check functions", "[accessors]")
{
     bc::bview uninit{};

     SECTION("holds_alternative") {
         SECTION("enum/tag based") {
            CHECK(holds_alternative<bc::btype::uninitialized>(uninit));
            CHECK(holds_alternative<bc::btype::integer>(i_view_const));
            CHECK(holds_alternative<bc::btype::string>(s_view_const));
            CHECK(holds_alternative<bc::btype::list>(l_view_const));
            CHECK(holds_alternative<bc::btype::dict>(d_view_const));

            CHECK_FALSE(holds_alternative<bc::btype::list>(s_view_const));
        }

        SECTION("type based") {
            CHECK(holds_alternative<bc::integer_bview>(i_view_const));
            CHECK(holds_alternative<bc::string_bview>(s_view_const));
            CHECK(holds_alternative<bc::list_bview>(l_view_const));
            CHECK(holds_alternative<bc::dict_bview>(d_view_const));
            CHECK_FALSE(holds_alternative<bc::dict_bview>(s_view_const));
        }
     }

    SECTION("is_uninitialised") {
        auto b = holds_uninitialized(uninit);
        CHECK(b);
        CHECK_FALSE(holds_integer(uninit));
        CHECK_FALSE(holds_string(uninit));
        CHECK_FALSE(holds_list(uninit));
        CHECK_FALSE(holds_dict(uninit));
    }
    SECTION("is_integer") {
        auto b = holds_integer(i_view_const);
        CHECK(b);
        CHECK_FALSE(holds_string(i_view_const));
        CHECK_FALSE(holds_list(i_view_const));
        CHECK_FALSE(holds_dict(i_view_const));
    }
    SECTION("is_string") {
        auto b = holds_string(s_view_const);
        CHECK(b);
        CHECK_FALSE(holds_integer(s_view_const));
        CHECK_FALSE(holds_list(s_view_const));
        CHECK_FALSE(holds_dict(s_view_const));
    }
    SECTION("is_list") {
        auto b = holds_list(l_view_const);
        CHECK(b);
        CHECK_FALSE(holds_integer(l_view_const));
        CHECK_FALSE(holds_string(l_view_const));
        CHECK_FALSE(holds_dict(l_view_const));
    }
    SECTION("is_dict") {
        auto b = holds_dict(d_view_const);
        CHECK(b);
        CHECK_FALSE(holds_integer(d_view_const));
        CHECK_FALSE(holds_string(d_view_const));
        CHECK_FALSE(holds_list(d_view_const));
    }
}
