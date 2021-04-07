
#include <experimental/source_location>

#include <catch2/catch.hpp>
#include <fmt/format.h>
#include <CLI/CLI.hpp>

#include <dottorrent/dht_node.hpp>
#include "common.hpp"
#include "tracker_database.hpp"
#include "test_resources.hpp"

namespace dt = dottorrent;
namespace tt = torrenttools;

#define PARSE_ARGS(cmd) \
try { app.parse(cmd); } catch (CLI::ParseError& e) { FAIL(e.what()); } \

#define PARSE_ARGS_THROWING(cmd) \
app.parse(cmd)

TEST_CASE("test main app argument parsing")
{
    const auto* tracker_database = tt::load_tracker_database();

    auto source = std::experimental::source_location::current();
    auto file = source.file_name();

    CLI::App app("test app", "torrenttools");
    auto create_app = app.add_subcommand("create",   "Create a new metafile");
    create_app_options create_options {};
    configure_main_app(create_app, create_options);

    SECTION("test announce option")
    {
        std::string tracker1 = "https://test1.com/announce.php";
        std::string tracker2 = "https://test2.com/announce.php";
        std::string tracker3 = "https://test3.com/announce.php";
        std::string tracker_abbr1 = "HDB";
        std::string tracker_abbrv1_url = tracker_database->at(tracker_abbr1).announce_url;
    }
}