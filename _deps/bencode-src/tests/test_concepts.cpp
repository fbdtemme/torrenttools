#include <string_view>
#include <vector>

#include <catch2/catch.hpp>
#include <bencode/traits/all.hpp>
#include <bencode/detail/concepts.hpp>

TEST_CASE("test serialisation_traits")
{
    using namespace bencode;
    CHECK(serializable_to<bool,          bencode_type::integer>);
    CHECK(serializable_to<std::int8_t,   bencode_type::integer>);
    CHECK(serializable_to<std::int16_t,  bencode_type::integer>);
    CHECK(serializable_to<std::int32_t,  bencode_type::integer>);
    CHECK(serializable_to<std::int64_t,  bencode_type::integer>);
    CHECK(serializable_to<std::uint8_t,  bencode_type::integer>);
    CHECK(serializable_to<std::uint16_t, bencode_type::integer>);
    CHECK(serializable_to<std::uint32_t, bencode_type::integer>);
    CHECK(serializable_to<std::uint64_t, bencode_type::integer>);

    CHECK(serializable_to<const char*,       bencode_type::string>);
    CHECK(serializable_to<const char[5],     bencode_type::string>);
    CHECK(serializable_to<std::string,       bencode_type::string>);
    CHECK(serializable_to<std::string_view,  bencode_type::string>);

    CHECK(serializable_to<std::array<int, 3>,      bencode_type::list>);
    CHECK(serializable_to<std::vector<int>,        bencode_type::list>);
    CHECK(serializable_to<std::forward_list<int>,  bencode_type::list>);
    CHECK(serializable_to<std::list<int>,          bencode_type::list>);
    CHECK(serializable_to<std::deque<int>,         bencode_type::list>);
    CHECK(serializable_to<std::set<int>,           bencode_type::list>);
    CHECK(serializable_to<std::multiset<int>,      bencode_type::list>);
    CHECK(serializable_to<std::unordered_set<int>,      bencode_type::list>);
    CHECK(serializable_to<std::unordered_multiset<int>, bencode_type::list>);

    CHECK(serializable_to<std::map<std::string, int>,                bencode_type::dict>);
    CHECK(serializable_to<std::multimap<std::string, int>,           bencode_type::dict>);
    CHECK(serializable_to<std::unordered_map<std::string, int>,      bencode_type::dict>);
    CHECK(serializable_to<std::unordered_multimap<std::string, int>, bencode_type::dict>);
}

