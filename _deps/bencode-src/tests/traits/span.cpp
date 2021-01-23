#include <catch2/catch.hpp>

#include <bencode/bencode.hpp>
#include <bencode/detail/concepts.hpp>
#include <bencode/traits/all.hpp>
#include <iostream>

#include "../bview/data.hpp"


TEST_CASE("span: serializable_to", "[traits-span]")
{
    using namespace bencode;

    SECTION("to string") {
        CHECK(serializable_to<std::span<char>, bencode_type::string>);
        CHECK(serializable_to<std::span<const char>, bencode_type::string>);
        CHECK(serializable_to<std::span<std::byte>, bencode_type::string>);
        CHECK(serializable_to<std::span<const std::byte>, bencode_type::string>);
    }

    SECTION("to list") {
        CHECK(serializable_to<std::span<int>, bencode_type::list>);
    }
}

TEST_CASE("span: connect",  "[traits-span]")
{
    using namespace bencode;
    using namespace std::string_view_literals;
    auto consumer = bencode::events::to_bvalue{};

    SECTION("span<char>") {
        auto string = "test"sv;
        auto s = std::span(string);
        bencode::connect(consumer, s);
        CHECK(consumer.value() == "test");
    }

    SECTION("span<const char>") {
        const auto string = "test"sv;
        auto s = std::span(string);
        bencode::connect(consumer, s);
        CHECK(consumer.value() == "test");
    }
    SECTION("span<std::byte>") {
        auto data = std::vector<std::byte>(
                { std::byte{'a'}, std::byte{'b'}, std::byte{'c'}});

        bencode::connect(consumer, std::span(data));
        auto v = consumer.value();
        CHECK(bc::holds_string(v));
        CHECK(v == "abc");
    }
    SECTION("span<const std::byte>") {
        const auto data = std::vector<std::byte>(
                { std::byte{'a'}, std::byte{'b'}, std::byte{'c'}}
        );

        bencode::connect(consumer, std::span(data));
        auto v = consumer.value();
        CHECK(bc::holds_string(v));
        CHECK(v == "abc");
    }
    SECTION("span<const int>") {
        const auto data = std::array<int, 3>({1, 2, 3});
        bencode::connect(consumer, std::span(data));
        auto v = consumer.value();
        CHECK(bc::holds_list(v));
    }
}

TEST_CASE("span: bvalue construction", "[traits-span]")
{
    using namespace bencode;

    SECTION ("string") {
        const auto data = std::vector<std::byte>(
                {std::byte{'a'}, std::byte{'b'}, std::byte{'c'}}
        );

        const bc::bvalue b{std::span(data)};
        CHECK(bc::holds_string(b));
    }

    SECTION("list") {
        const auto data = std::array<int, 3>({1, 2, 3});
        const bc::bvalue b{std::span(data)};
        CHECK(bc::holds_list(b));
    }

}

TEST_CASE("span: bvalue assignment",  "[traits-span]")
{
    SECTION ("string") {
        const auto data = std::vector<std::byte>(
                {std::byte{'a'}, std::byte{'b'}, std::byte{'c'}}
        );

        bc::bvalue b {};
        b = std::span(data);
        CHECK(bc::holds_string(b));
    }

    SECTION("list") {
        const auto data = std::array<int, 3>({1, 2, 3});

        bc::bvalue b {};
        b = std::span(data);
        CHECK(bc::holds_list(b));
    }
}

TEST_CASE("span: compare with bvalue",  "[traits-span]")
{
    using namespace bencode;
    bencode::bvalue bv {};

    const std::string c_equal_data {"cccc"};
    const std::string c_less_data {"aaaa"};
    const std::string c_greater_data {"zzzz"};

    auto v = bc::bvalue("cccc");

    auto c_equal = std::span(c_equal_data);
    auto c_less = std::span(c_less_data);
    auto c_greater = std::span(c_greater_data);

    CHECK(v < c_greater);       CHECK(c_greater > v);
    CHECK(v <= c_greater);      CHECK(c_greater >= v);
    CHECK(v <= c_equal);        CHECK(c_equal >= v);
    CHECK(v == c_equal);        CHECK(c_equal == v);
    CHECK(v >= c_equal);        CHECK(c_equal <= v);
    CHECK(v >= c_less);         CHECK(c_less <= v);
    CHECK(v > c_less);          CHECK(c_less < v);
}

TEST_CASE("span: compare with bview",  "[traits-span]")
{
    using namespace bencode;
    bencode::bvalue bv {};

    const std::string c_equal_data {"spam"};
    const std::string c_less_data {"aaaa"};
    const std::string c_greater_data {"zzzz"};

    auto v = s_view;

    auto c_equal = std::span(c_equal_data);
    auto c_less = std::span(c_less_data);
    auto c_greater = std::span(c_greater_data);

    CHECK(v < c_greater);       CHECK(c_greater > v);
    CHECK(v <= c_greater);      CHECK(c_greater >= v);
    CHECK(v <= c_equal);        CHECK(c_equal >= v);
    CHECK(v == c_equal);        CHECK(c_equal == v);
    CHECK(v >= c_equal);        CHECK(c_equal <= v);
    CHECK(v >= c_less);         CHECK(c_less <= v);
    CHECK(v > c_less);          CHECK(c_less < v);
}



TEST_CASE("span: retrieve from bvalue",  "[traits-span]")
{
    using namespace bencode;

    SECTION("string - const char") {
        auto bv = bvalue("long test string to allocated on the heap");
        auto t = get_as<std::span<const char>>(bv);
        auto t1 = get_as<std::span<char>>(bv);

        CHECK(t == bv);
    }

    SECTION("string - const std::byte") {
        auto bv = bvalue("long test string to allocated on the heap");
        auto t = get_as<std::span<const std::byte>>(bv);
        auto t1 = get_as<std::span<std::byte>>(bv);
        CHECK(t == bv);
    }
}

TEST_CASE("span: retrieve from bview",  "[traits-span]")
{
    using namespace bencode;

    SECTION("string - const char") {
        auto t = get_as<std::span<const char>>(s_view);
        CHECK(t == s_view);
    }

    SECTION("string - const std::byte") {
        auto t = get_as<std::span<const std::byte>>(s_view);
        CHECK(t == s_view);
    }
}