//
// Created by fbdtemme on 18/07/19.
//
#include <filesystem>
#include <fstream>

#include <catch2/catch.hpp>
#include "dottorrent/metafile.hpp"
#include <iostream>

namespace fs = std::filesystem;


TEST_CASE("decode .torrent file")
{
    auto path = fs::path(TEST_DIR"/resources/Fedora-Workstation-Live-x86_64-30.torrent");
    std::cout << path << std::endl;

    if (!fs::exists(path)) {
        throw std::invalid_argument("path does not exist");
    }

    SECTION("parse") {
        std::ifstream fs(path, std::ios::binary);
        auto m = dottorrent::load_metafile(path);
        SUCCEED();
    }
}

TEST_CASE("test roundtrip")
{
    auto path = fs::path(TEST_DIR"/resources/Fedora-Workstation-Live-x86_64-30.torrent");
    std::cout << path << std::endl;

    if (!fs::exists(path)) {
        throw std::invalid_argument("path does not exist");
    }
    SECTION("parse flat/non-owning") {
        std::ifstream fs(path, std::ios::binary);
        const auto s = std::string(std::istreambuf_iterator<char>(fs), std::istreambuf_iterator<char>());
        auto m1 = dottorrent::read_metafile(s);

        std::ostringstream ss {};
        dottorrent::write_metafile_to(ss, m1);
        const auto t = ss.str();
        auto m2 = dottorrent::read_metafile(t);
        CHECK(m1 == m2);
    }
}