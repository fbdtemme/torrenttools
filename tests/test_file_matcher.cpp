#include <catch2/catch.hpp>
#include <filesystem>

#include "file_matcher.hpp"

namespace fs = std::filesystem;

bool contains(std::vector<fs::path> v, const fs::path& path)
{
    auto it = std::find(v.begin(), v.end(), path);
    return it != v.end();
}

static const auto main_cpp              = fs::path(TEST_DIR) / "main.cpp";
static const auto cmakelists_txt        = fs::path(TEST_DIR) / "CMakeLists.txt";
static const auto test_file_matcher_cpp = fs::path(TEST_DIR) / "test_file_matcher.cpp";
static const auto test_info_cpp         = fs::path(TEST_DIR) / "test_info.cpp";
static const auto src_create_cpp        = fs::path(TEST_DIR) / "../src/create.cpp";
static const auto fedora_torrent        = fs::path(TEST_DIR) / "resources/Fedora-Workstation-Live-x86_64-30.torrent";


TEST_CASE("test file_matcher")
{
    torrenttools::file_matcher matcher{};

    SECTION("test block extension") {
        matcher.block_extension("txt");
        auto files = matcher.run(fs::path(TEST_DIR));

        CHECK(contains(files, main_cpp));
        CHECK_FALSE(contains(files, cmakelists_txt));
    }

    SECTION("test allow extension") {
        matcher.allow_extension("cpp");
        auto files = matcher.run(fs::path(TEST_DIR));

        CHECK(contains(files, main_cpp));
        CHECK_FALSE(contains(files, cmakelists_txt));
    }

    SECTION("test block extension") {
        matcher.block_extension("txt");
        auto files = matcher.run(fs::path(TEST_DIR));

        CHECK(contains(files, main_cpp));
        CHECK_FALSE(contains(files, cmakelists_txt));
    }

    SECTION("test allow pattern") {
        SECTION("valid pattern") {
            matcher.include_pattern(".*test_.*.cpp");

            auto files = matcher.run(fs::path(TEST_DIR));

            CHECK_FALSE(contains(files, main_cpp));
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

            auto files = matcher.run(fs::path(TEST_DIR));

            CHECK(contains(files, main_cpp));
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

        auto files = matcher.run(fs::path(TEST_DIR));
        CHECK(contains(files, test_info_cpp));
        CHECK_FALSE(contains(files, cmakelists_txt));
        CHECK_FALSE(contains(files, test_file_matcher_cpp));
    }

    SECTION("test exclude directory")
    {
        matcher.exclude_directory("resources");

        auto files = matcher.run(fs::path(TEST_DIR));
        CHECK(contains(files, test_info_cpp));
        CHECK(contains(files, test_file_matcher_cpp));
        CHECK_FALSE(contains(files, fedora_torrent));
    }
}