
#include <experimental/source_location>

#include <catch2/catch.hpp>
#include <fmt/format.h>
#include <CLI/CLI.hpp>

#include <dottorrent/dht_node.hpp>
#include "pad.hpp"
#include "tracker_database.hpp"

#include "test_resources.hpp"

namespace dt = dottorrent;
namespace tt = torrenttools;

#define PARSE_ARGS(cmd) \
try { app.parse(cmd); } catch (CLI::ParseError& e) { FAIL(e.what()); } \

#define PARSE_ARGS_THROWING(cmd) \
app.parse(cmd)

TEST_CASE("test pad app argument parsing")
{
    auto source = std::experimental::source_location::current();
    auto file = source.file_name();

    CLI::App app("test app", "torrenttools");
    auto pad_app = app.add_subcommand("pad",   "Create a new metafile");
    pad_app_options options {};
    configure_pad_app(pad_app, options);

    SECTION("target metafile") {
        auto cmd = fmt::format("pad {} {}", TEST_RESOURCES_DIR, fedora_torrent);
        PARSE_ARGS(cmd);

        CHECK(options.metafile == fedora_torrent);
        CHECK(options.target == TEST_RESOURCES_DIR);
    }

    SECTION("target dir does not exist") {
        auto cmd = fmt::format("pad {} {}", "blah", fedora_torrent);
        CHECK_THROWS(PARSE_ARGS_THROWING(cmd));
    }

    SECTION("target metafile does not exist") {
        auto cmd = fmt::format("pad {} {}", "blah", fedora_torrent);
        CHECK_THROWS(PARSE_ARGS_THROWING(cmd));
    }
}

TEST_CASE("test pad app")
{
    temporary_directory tmp_dir {};
    fs::path target = fs::path(tmp_dir) / "test-pad";
    pad_app_options options {};
    main_app_options main_options;

    options.metafile = bittorrent_hybrid;
    options.target = target;
    run_pad_app(main_options, options);

    // Test all padding files
    auto m = dt::load_metafile(options.metafile);
    for (const auto& f: m.storage()) {
        if (!f.is_padding_file()) continue;
        CHECK(fs::exists(target / f.path()));
        CHECK(fs::file_size(target / f.path()) ==  f.file_size());
    }
}

