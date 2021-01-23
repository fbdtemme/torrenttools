#include <catch2/catch.hpp>
#include <filesystem>
#include <dottorrent/serialization/all.hpp>
#include <bencode/bencode.hpp>

namespace fs = std::filesystem;

TEST_CASE("std::filesystem::path serialization")
{
    fs::path path {"dir/file.txt" };
    std::string result {};
    bencode::encode_to(std::back_inserter(result), path);
    CHECK(result == "l3:dir8:file.txte");
}

TEST_CASE("announce_url_list serialization")
{
    std::string result {};
    dottorrent::announce_url_list l {};
    l.emplace("https://test.com", 0);
    bencode::encode_to(std::back_inserter(result), l);
    CHECK(result == "ll16:https://test.comee");
}

TEST_CASE("dht_node serialization")
{
    std::string result {};
    dottorrent::dht_node node{"https://test.com", 5050};
    bencode::encode_to(std::back_inserter(result), node);
    CHECK(result == "l16:https://test.comi5050ee");
}