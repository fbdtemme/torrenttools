#include <bencode/bview.hpp>
#include <bencode/detail/events/encode_json_to.hpp>

#include <catch2/catch.hpp>

#include <iostream>
#include <fstream>
#include <string_view>
#include <sstream>

#include "data.hpp"

using namespace std::string_view_literals;
using namespace bencode;


TEST_CASE("test descriptor parser - simd", "[parser]")
{
    using namespace bencode;

    auto p = descriptor_parser();
    auto r = p.parse(example);

    CHECK(r);
    auto index = std::move(r->descriptors());
    CHECK_FALSE(index.empty());

    CHECK(index.size() == 19);
    // check outer dict

    CHECK(index[0].is_dict_begin());
    CHECK(index[0].position() == 0);
    CHECK(index[0].offset() == 18);

    CHECK(index[18].is_dict_end());
    CHECK(index[18].position() == example.size()-1);
    CHECK(index[18].offset() == 18);

    // check first key bvalue pair ["one", 1]
    auto& k1 = index[1];
    auto& v1 = index[2];
    CHECK(k1.is_string());
    CHECK(k1.is_dict_key());
    CHECK(k1.position() == 1);
    CHECK(k1.offset() == 2);
    CHECK(v1.is_integer());
    CHECK(v1.is_dict_value());
    CHECK(v1.value() == 1);

    // check second key bvalue pair ["one", [{"bar": 0, "foo": 0}]]
    auto& k2 = index[3];
    auto& v2 = index[4];
    CHECK(k2.is_string());
    CHECK(k2.is_dict_key());
    CHECK(k2.position() == 9);
    CHECK(k2.offset() == 2);
    CHECK(v2.is_list());
    CHECK(v2.is_dict_value());
    CHECK(v2.position() == 16);
    CHECK(v2.size() == 1);
}


TEST_CASE("descriptor parser", "[descriptor_parser]")
{
    auto parser = bencode::descriptor_parser();

    SECTION("valid torrents") {
        auto resource = GENERATE_COPY(values({
                RESOURCES_DIR"/COVID-19-image-dataset-collection.torrent",
                RESOURCES_DIR"/Fedora-Workstation-Live-x86_64-30.torrent",
                RESOURCES_DIR"/NASA-Viking-Merged-Color-Mosaic.torrent",
        }));

        std::ifstream ifs(resource, std::ifstream::binary);
        std::string torrent(
                std::istreambuf_iterator<char>{ifs},
                std::istreambuf_iterator<char>{});

        auto r = parser.parse(torrent);
        CHECK_FALSE(parser.has_error());
        CHECK(r.has_value());
    }

    SECTION("invalid dict value") {
        auto r = parser.parse(expected_dict_value);
        CHECK_FALSE(r);
        CHECK(parser.error().errc() == parsing_errc::expected_dict_value);
    }

    SECTION("error - recursion limit - list") {
        auto parser2 = bencode::descriptor_parser({.recursion_limit=10});
        auto r = parser2.parse(recursion_limit_list);
        CHECK_FALSE(r);
        CHECK(parser2.error().errc() == parsing_errc::recursion_depth_exceeded);
    }

    SECTION("error - recursion limit - dict") {
        auto parser2 = bencode::descriptor_parser({.recursion_limit=10});
        auto r = parser2.parse(recursion_limit_dict);
        CHECK_FALSE(r);
        CHECK(parser2.error().errc() == parsing_errc::recursion_depth_exceeded);
    }

    SECTION("error - value limit") {
        auto parser2 = bencode::descriptor_parser({.value_limit=10});
        auto r = parser2.parse(sintel_torrent);
        REQUIRE_FALSE(r);
        CHECK(parser2.error().errc() == parsing_errc::value_limit_exceeded);
    }
    SECTION("error - integer parsing") {
        auto r = parser.parse(error_integer);
        REQUIRE_FALSE(r);
        CHECK(parser.error().errc() == parsing_errc::leading_zero);
    }
    SECTION("error - string parsing") {
        auto r = parser.parse(error_string);
        REQUIRE_FALSE(r);
        CHECK(parser.error().errc() == parsing_errc::unexpected_eof);
    }
    SECTION("error - dict key parsing") {
        auto r = parser.parse(error_dict_key);
        REQUIRE_FALSE(r);
        CHECK(parser.error().errc() == parsing_errc::expected_colon);
    }
    SECTION("error - missing end - list") {
        auto r = parser.parse(error_missing_end_list);
        REQUIRE_FALSE(r);
        CHECK(parser.error().errc() == parsing_errc::expected_list_value_or_end);
    }
    SECTION("error - missing end - dict") {
        auto r = parser.parse(error_missing_end_dict);
        REQUIRE_FALSE(r);
        CHECK(parser.error().errc() == parsing_errc::expected_dict_key_or_end);
    }
    SECTION("error - missing value") {
        auto r = parser.parse(error_missing_value);
        REQUIRE_FALSE(r);
        CHECK(parser.error().errc() == parsing_errc::expected_value);
    }
    SECTION("error - missing dict value") {
        auto r = parser.parse(error_missing_dict_value);
        REQUIRE_FALSE(r);
        CHECK(parser.error().errc() == parsing_errc::expected_dict_value);
    }
    SECTION("error - missing list value or end") {
        auto r = parser.parse(error_missing_list_value_or_end);
        REQUIRE_FALSE(r);
        CHECK(parser.error().errc() == parsing_errc::expected_list_value_or_end);
    }
    SECTION("error - error_missing_dict_key_or_end") {
        auto r = parser.parse(error_missing_dict_key_or_end);
        CHECK_FALSE(r);
        CHECK(parser.error().errc() == parsing_errc::expected_dict_key_or_end);
    }
}

TEST_CASE("descriptor parser - example, sintel")
{
    const auto [data, expected] = GENERATE_COPY(table<std::string_view, std::string_view>({
            {example,        example_json_result},
            {sintel_torrent, sintel_json_result}
    }));

    std::string out {};
    auto parser = descriptor_parser();
    auto r = parser.parse(data);
    CHECK(r);
    auto desc = r->get_root();
    auto consumer = bencode::events::encode_json_to(std::back_inserter(out));
    bencode::connect(consumer, desc);
    CHECK(out == expected);
}
