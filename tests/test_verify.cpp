
#include <experimental/source_location>

#include <catch2/catch_all.hpp>
#include <fmt/format.h>
#include <CLI/CLI.hpp>

#include <dottorrent/dht_node.hpp>
#include "create.hpp"
#include "verify.hpp"
#include "tracker_database.hpp"
#include "test_resources.hpp"

namespace dt = dottorrent;
namespace tt = torrenttools;

#define PARSE_ARGS(cmd) \
try { app.parse(cmd); } catch (CLI::ParseError& e) { FAIL(e.what()); } \

#define PARSE_ARGS_THROWING(cmd) \
app.parse(cmd)


TEST_CASE("Test verify app argument parsing")
{
    fs::path test_torrent = fs::path(TEST_RESOURCES_DIR) / "bittorrent-v2-test.torrent";
    fs::path test_target = fs::path(TEST_RESOURCES_DIR);

    CLI::App app("test app", "torrenttools");
    auto verify_app = app.add_subcommand("verify",   "Create a new metafile");
    verify_app_options verify_options {};
    configure_verify_app(verify_app, verify_options);

    SECTION("test metafile and target parsing") {
        SECTION("Valid metafile and target") {
            auto cmd = fmt::format("verify {} {}", test_torrent.string(), test_target.string());

            PARSE_ARGS(cmd);
            CHECK(verify_options.metafile == test_torrent);
            CHECK(verify_options.files_root_directory == test_target);
        }

        SECTION("Invalid metafile") {
            auto cmd = fmt::format("verify {} {}", "foo", test_target.string());
            CHECK_THROWS(PARSE_ARGS_THROWING(cmd));
        }

        SECTION("Invalid target") {
            auto cmd = fmt::format("verify {} {}", test_torrent.string(), "bar");
            CHECK_THROWS(PARSE_ARGS_THROWING(cmd));
        }
    }

    SECTION("test protocol") {
        SECTION("v1") {
            auto cmd = fmt::format("verify {} {} --protocol {}", test_torrent.string(), test_target.string(), "v1");

            PARSE_ARGS(cmd);
            CHECK(verify_options.protocol_version == dt::protocol::v1);
        }
        SECTION("v2") {
            auto cmd = fmt::format("verify {} {} --protocol {}", test_torrent.string(), test_target.string(), "v2");

            PARSE_ARGS(cmd);
            CHECK(verify_options.protocol_version == dt::protocol::v2);
        }
        SECTION("hybrid") {
            auto cmd = fmt::format("verify {} {} --protocol {}", test_torrent.string(), test_target.string(), "hybrid");

            PARSE_ARGS(cmd);
            CHECK(verify_options.protocol_version == dt::protocol::hybrid);
        }
    }

    SECTION("threads") {
        SECTION("default of 2") {
            auto cmd = fmt::format("verify {} {}", test_torrent.string(), test_target.string());
            PARSE_ARGS(cmd);
            CHECK(verify_options.threads == 2);
        }
        SECTION("option given") {
            auto cmd = fmt::format("verify {} {} --threads {}", test_torrent.string(), test_target.string(), "4");
            PARSE_ARGS(cmd);
            CHECK(verify_options.threads == 4);
        }
    }
}

TEST_CASE("test verify app: v1 torrent")
{
    temporary_directory tmp_dir {};
    main_app_options main_options {};
    verify_app_options verify_options {};

    SECTION("verify ") {
        verify_options.metafile = fs::path(TEST_RESOURCES_DIR) / "resources.torrent";
        verify_options.files_root_directory = fs::path(TEST_RESOURCES_DIR);
        verify_options.threads = 1;
        verify_options.protocol_version = dt::protocol::v1;
        run_verify_app(main_options, verify_options);
    }
}

TEST_CASE("test verify app: v2 torrent")
{
    temporary_directory tmp_dir {};
    main_app_options main_options {};

    SECTION("verify ") {
        verify_app_options verify_options {};

        verify_options.metafile = fs::path(TEST_RESOURCES_DIR) / "resources-hybrid.torrent";
        verify_options.files_root_directory = fs::path(TEST_RESOURCES_DIR);
        verify_options.threads = 1;
        verify_options.protocol_version = dt::protocol::v2;

        run_verify_app(main_options, verify_options);
    }
}