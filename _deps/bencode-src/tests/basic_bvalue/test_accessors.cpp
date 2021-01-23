#include <catch2/catch.hpp>

#include <type_traits>
#include <string>
#include <string_view>

#include "bencode/traits/all.hpp"
#include "bencode/bvalue.hpp"

using namespace std::string_literals;
using namespace std::string_view_literals;
using namespace bencode;


TEST_CASE("test get(bvalue)", "[accessors]")
{
    using namespace bencode;
    using vt = bencode::bencode_type;

    const bencode::bvalue const_integer(1);
    const bencode::bvalue const_string_view("test"sv);
    const bencode::bvalue const_string("test"s);
    const bencode::bvalue const_list(btype::list, {1, 2, 3});
    const bencode::bvalue const_dict(btype::dict, {{"test1", 1}, {"test2", "2"}});

    bencode::bvalue integer(1);
    bencode::bvalue string_view("test"sv);
    bencode::bvalue string("test"s);
    bencode::bvalue list(btype::list, {1, 2, 3});
    bencode::bvalue dict(btype::dict, {{"test1", 1}, {"test2", "2"}});

    SECTION("const reference") {
        const auto& i_ref = get<vt::integer>(const_integer);
        const auto& i_ref2 = get<vt::integer>(const_integer);
        const auto& s_ref = get<vt::string>(const_string);
        const auto& l_ref = get<vt::list>(const_list);
        const auto& m_ref = get<vt::dict>(const_dict);
    }

    SECTION("reference") {
        auto& i_ref = get<vt::integer>(integer);
        auto& s_ref = get<vt::string>(string);
        auto& l_ref = get<vt::list>(list);
        auto& m_ref = get<vt::dict>(dict);
    }

    SECTION("rvalue reference") {
        auto s_from_rvref = std::string(get<vt::string>(std::move(string)));
        auto l_from_rvref = std::vector(get<vt::list>(std::move(list)));
        auto m_from_rvref = std::map(get<vt::dict>(std::move(dict)));

        CHECK(get<vt::string>(string).empty());
        CHECK(get<vt::list>(list).empty());
        CHECK(get<vt::dict>(dict).empty());
    }
}


TEST_CASE("test get_if(bvalue)", "[accessors]")
{
    using namespace bencode;

    const bencode::bvalue const_integer(1);
    const bencode::bvalue const_string("test"s);
    const bencode::bvalue const_list(btype::list, {1, 2, 3});
    const bencode::bvalue const_dict(btype::dict, {{"test1", 1}, {"test2", "2"}});

    bencode::bvalue integer(1);
    bencode::bvalue string("test"s);
    bencode::bvalue list(btype::list, {1, 2, 3});
    bencode::bvalue dict(btype::dict, {{"test1", 1}, {"test2", "2"}});

    SECTION("const pointer - access by enum ") {
        const auto* i_ptr = get_if<btype::integer>(&const_integer);
        const auto* s_ptr = get_if<btype::string>(&const_string);
        const auto* l_ptr = get_if<btype::list>(&const_list);
        const auto* m_ptr = get_if<btype::dict>(&const_dict);
        CHECK(i_ptr);
        CHECK(s_ptr);
        CHECK(l_ptr);
        CHECK(m_ptr);
    }
    SECTION("const pointer - access by type ") {
        const auto* i_ptr = get_if<bvalue::integer_type>(&const_integer);
        const auto* s_ptr = get_if<bvalue::string_type>(&const_string);
        const auto* l_ptr = get_if<bvalue::list_type>(&const_list);
        const auto* m_ptr = get_if<bvalue::dict_type>(&const_dict);
        CHECK(i_ptr);
        CHECK(s_ptr);
        CHECK(l_ptr);
        CHECK(m_ptr);
    }

    SECTION("pointer - access by enum") {
        auto* i_ptr = get_if<btype::integer>(&integer);
        auto* s_ptr = get_if<btype::string>(&string);
        auto* l_ptr = get_if<btype::list>(&list);
        auto* m_ptr = get_if<btype::dict>(&dict);
        CHECK(i_ptr);
        CHECK(s_ptr);
        CHECK(l_ptr);
        CHECK(m_ptr);
    }

    SECTION("pointer - access by type ") {
        const auto* i_ptr = get_if<bvalue::integer_type>(&integer);
        const auto* s_ptr = get_if<bvalue::string_type>(&string);
        const auto* l_ptr = get_if<bvalue::list_type>(&list);
        const auto* m_ptr = get_if<bvalue::dict_type>(&dict);
        CHECK(i_ptr);
        CHECK(s_ptr);
        CHECK(l_ptr);
        CHECK(m_ptr);
    }
}


TEST_CASE("get_integer/get_if_integer (bview)", "[accessors]")
{
    bencode::bvalue b = 1;
    const bencode::bvalue b_const = 1;

    SECTION("reference") {
        auto& i = get_integer(b);
        CHECK(i == 1);
    }
    SECTION("const reference") {
        const auto& i = get_integer(b_const);
        CHECK(i == 1);
    }
    SECTION("pointer") {
        auto* i = get_if_integer(&b);
        CHECK(*i == 1);
    }
    SECTION("const pointer") {
        const auto* i = get_if_integer(&b_const);
        CHECK(*i == 1);
    }

    SECTION ("throws when accessing with wrong type") {
        REQUIRE_THROWS_AS(get<btype::string>(b), bad_bvalue_access);
    }
}

TEST_CASE("get_string/get_if_string (bview)", "[accessors]")
{
    bencode::bvalue b = "test";
    const bencode::bvalue b_const = "test";
    std::string ref = "test";

    SECTION ("reference") {
        auto& l = get_string(b);
        CHECK(l == bvalue::string_type("test"));
    }
    SECTION ("rvalue reference") {
        bencode::bvalue b2 = "long test string to check for move";

        auto l = get_string(std::move(b2));
        CHECK(l.starts_with("long test"));
        CHECK(get_string(b2).empty());
    }
    SECTION("pointer") {
        auto* i = get_if_string(&b);
        CHECK(*i == ref);
    }
    SECTION("const pointer") {
        const auto* i = get_if_string(&b_const);
        CHECK(*i == ref);
    }

    SECTION ("throws bad_bvalue_access when accessing with wrong type") {
        REQUIRE_THROWS_AS(get<bencode_type::integer>(b), bad_bvalue_access);
    }
}


TEST_CASE("get_list/get_if_list (bview)", "[accessors]")
{
    bencode::bvalue b(btype::list, {1, 2, 3, 4});
    const bencode::bvalue b_const(btype::list, {1, 2, 3, 4});
    auto ref = bvalue::list_type{1, 2, 3, 4};

    SECTION ("reference") {} {
        auto& l = get_list(b);
        CHECK(l == ref);
    }

    SECTION ("rvalue reference") {
        auto l = get_list(std::move(b));
        CHECK(l == ref);
        CHECK(get_list(b).empty());
    }

    SECTION("pointer") {
        auto* i = get_if_list(&b);
        CHECK(*i == ref);
    }
    SECTION("const pointer") {
        const auto* i = get_if_list(&b_const);
        CHECK(*i == ref);
    }

    SECTION ("throws when accessing with wrong type") {
        REQUIRE_THROWS_AS(get<btype::integer>(b), bad_bvalue_access);
    }
}

TEST_CASE("get_dict/get_if_dict (bview)", "[accessors]")
{
    bencode::bvalue b(btype::dict, {{"test", 1}});
    const bencode::bvalue b_const(btype::dict, {{"test", 1}});
    auto ref = bvalue::dict_type{{"test", 1}};

    SECTION ("reference") {
        auto& l = get_dict(b);
        CHECK(l == ref);
    }

    SECTION ("rvalue reference") {
        auto l = get_dict(std::move(b));
        CHECK(l == ref);
        CHECK(get_dict(b).empty());
    }
    SECTION("pointer") {
        auto* i = get_if_dict(&b);
        CHECK(*i == ref);
    }
    SECTION("const pointer") {
        const auto* i = get_if_dict(&b_const);
        CHECK(*i == ref);
    }

    SECTION ("throws bad_bvalue_access when accessing with wrong type") {
        REQUIRE_THROWS_AS(get<btype::integer>(b), bad_bvalue_access);
    }
}

TEST_CASE("test current alternative check functions (bview)", "[accessors]")
{
     bencode::bvalue uninit{};
     bencode::bvalue integer(1);
     bencode::bvalue string_view("test"sv);
     bencode::bvalue string("test"s);
     bencode::bvalue list(btype::list, {1, 2, 3});
     bencode::bvalue dict(btype::dict, {{"test1", 1}, {"test2", "2"}});

    SECTION("is_uninitialised") {
        auto b = holds_uninitialized(uninit);
        CHECK(b);
        CHECK_FALSE(holds_integer(uninit));
        CHECK_FALSE(holds_string(uninit));
        CHECK_FALSE(holds_list(uninit));
        CHECK_FALSE(holds_dict(uninit));
    }
    SECTION("is_integer") {
        auto b = holds_integer(integer);
        CHECK(b);
        CHECK_FALSE(holds_string(integer));
        CHECK_FALSE(holds_list(integer));
        CHECK_FALSE(holds_dict(integer));
    }
    SECTION("is_string") {
        auto b = holds_string(string);
        CHECK(b);
        CHECK_FALSE(holds_integer(string));
        CHECK_FALSE(holds_list(string));
        CHECK_FALSE(holds_dict(string));
    }
    SECTION("is_list") {
        auto b = holds_list(list);
        CHECK(b);
        CHECK_FALSE(holds_integer(list));
        CHECK_FALSE(holds_string(list));
        CHECK_FALSE(holds_dict(list));
    }
    SECTION("is_dict") {
        auto b = holds_dict(dict);
        CHECK(b);
        CHECK_FALSE(holds_integer(dict));
        CHECK_FALSE(holds_string(dict));
        CHECK_FALSE(holds_list(dict));
    }

}
