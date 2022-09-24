#include <catch2/catch_all.hpp>
#include <filesystem>

#include "file_matcher.hpp"

namespace fs = std::filesystem;

bool contains(std::vector<fs::path> v, const fs::path& path)
{
    auto it = std::find(v.begin(), v.end(), path);
    return it != v.end();
}

static const auto test_create_cpp       = fs::path(TEST_DIR) / "test_create.cpp";
static const auto cmakelists_txt        = fs::path(TEST_DIR) / "CMakeLists.txt";
static const auto test_file_matcher_cpp = fs::path(TEST_DIR) / "test_file_matcher.cpp";
static const auto test_info_cpp         = fs::path(TEST_DIR) / "test_info.cpp";
static const auto src_create_cpp        = fs::path(TEST_DIR) / "../src/create.cpp";
static const auto fedora_torrent        = fs::path(TEST_DIR) / "resources/Fedora-Workstation-Live-x86_64-30.torrent";


TEST_CASE("test file_matcher")
{
    torrenttools::file_matcher matcher{};

    SECTION("test block extension") {
        matcher.set_search_root(fs::path(TEST_DIR));
        matcher.block_extension("txt");

        matcher.start();
        matcher.wait();
        auto files = matcher.results();

        CHECK(contains(files, test_create_cpp));
        CHECK_FALSE(contains(files, cmakelists_txt));
    }

    SECTION("test allow extension") {
        matcher.set_search_root(fs::path(TEST_DIR));
        matcher.allow_extension("cpp");

        matcher.start();
        matcher.wait();
        auto files = matcher.results();

        CHECK(contains(files, test_create_cpp));
        CHECK_FALSE(contains(files, cmakelists_txt));
    }

    SECTION("test block extension") {
        matcher.set_search_root(fs::path(TEST_DIR));
        matcher.block_extension("txt");

        matcher.start();
        matcher.wait();
        auto files = matcher.results();

        CHECK(contains(files, test_create_cpp));
        CHECK_FALSE(contains(files, cmakelists_txt));
    }

    SECTION("test allow pattern") {
        SECTION("valid pattern") {
            matcher.include_pattern(".*test_.*.cpp");
            matcher.set_search_root(fs::path(TEST_DIR));
            matcher.start();
            matcher.wait();
            auto files = matcher.results();

            CHECK_FALSE(contains(files, cmakelists_txt));
            CHECK(contains(files, test_file_matcher_cpp));
        }
        SECTION("invalid pattern")
        {
            CHECK_THROWS_AS(matcher.include_pattern(".**(.*.cpp"), std::invalid_argument);
        }
    }

    SECTION("test block pattern") {
        SECTION("valid pattern") {
            matcher.exclude_pattern(".*test_.*.cpp");
            matcher.set_search_root(fs::path(TEST_DIR));
            matcher.start();
            matcher.wait();
            auto files = matcher.results();

            CHECK(contains(files, cmakelists_txt));
            CHECK_FALSE(contains(files, test_file_matcher_cpp));
        }
        SECTION("invalid pattern")
        {
            CHECK_THROWS_AS(matcher.include_pattern(".**(.*.cpp"), std::invalid_argument);
        }
    }

    SECTION("test whitelist / blacklist combination")
    {
        matcher.include_pattern(".*test_.*");
        matcher.exclude_pattern(".*matcher.*");
        matcher.set_search_root(fs::path(TEST_DIR));
        matcher.start();
        matcher.wait();
        auto files = matcher.results();
        CHECK(contains(files, test_info_cpp));
        CHECK_FALSE(contains(files, cmakelists_txt));
        CHECK_FALSE(contains(files, test_file_matcher_cpp));
    }

    SECTION("test exclude directory")
    {
        matcher.exclude_directory("resources");
        matcher.set_search_root(fs::path(TEST_DIR));
        matcher.start();
        matcher.wait();
        auto files = matcher.results();
        CHECK(contains(files, test_info_cpp));
        CHECK(contains(files, test_file_matcher_cpp));
        CHECK_FALSE(contains(files, fedora_torrent));
    }
}