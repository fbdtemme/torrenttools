
#include <experimental/source_location>

#include <catch2/catch.hpp>
#include <fmt/format.h>
#include <CLI/CLI.hpp>

#include <dottorrent/dht_node.hpp>
#include "magnet.hpp"
#include "tracker_database.hpp"

#include "test_resources.hpp"

namespace dt = dottorrent;
namespace tt = torrenttools;

#define PARSE_ARGS(cmd) \
try { app.parse(cmd); } catch (CLI::ParseError& e) { FAIL(e.what()); } \

#define PARSE_ARGS_THROWING(cmd) \
app.parse(cmd)


TEST_CASE("test magnet app argument parsing")
{
    auto source = std::experimental::source_location::current();
    auto file = source.file_name();

    CLI::App app("test app", "torrenttools");
    auto edit_app = app.add_subcommand("magnet",  "test app");
    magnet_app_options magnet_options {};
    configure_magnet_app(edit_app, magnet_options);

    SECTION("--protocol")
    {
        SECTION("v1") {
            auto cmd = fmt::format("magnet {} --protocol v1", file);
            PARSE_ARGS(cmd);
            CHECK(magnet_options.protocol == dt::protocol::v1);
        }
        SECTION("v2") {
            auto cmd = fmt::format("magnet {} --protocol v2", file);
            PARSE_ARGS(cmd);
            CHECK(magnet_options.protocol == dt::protocol::v2);
        }
        SECTION("hybrid - default") {
            auto cmd = fmt::format("magnet {}", file);
            PARSE_ARGS(cmd);
            CHECK(magnet_options.protocol == dt::protocol::hybrid);
        }
        SECTION("hybrid - explicit") {
            auto cmd = fmt::format("magnet {} --protocol hybrid", file);
            PARSE_ARGS(cmd);
            CHECK(magnet_options.protocol == dt::protocol::hybrid);
        }
    }
}

TEST_CASE("test magnet command")
{
    std::stringstream buffer {};
    auto redirect_guard = cout_redirect(buffer.rdbuf());

    SECTION("v1 torrent") {
        magnet_app_options options {};
        options.metafile = ubuntu_torrent;

        run_magnet_app(options);
        CHECK(buffer.str() == "magnet:?xt=urn:btih:36c67464c37a83478ceff54932b5a9bddea636f3"
                              "&dn=ubuntu-20.04.1-live-server-amd64.iso"
                              "&tr=https%3A%2F%2Ftorrent.ubuntu.com%2Fannounce"
                              "&tr=https%3A%2F%2Fipv6.torrent.ubuntu.com%2Fannounce\n");
    }

    SECTION("v2 torrent") {
        magnet_app_options options {};
        options.metafile = bittorrent_v2;

        run_magnet_app(options);
        CHECK(buffer.str() == "magnet:?xt=urn:btmh:1220caf1e1c30e81cb361b9ee167c4aa64228a7fa4fa9f6105232b28ad099f3a302e"
                              "&dn=bittorrent-v2-test\n");
    }

    SECTION("hybrid torrent") {
        magnet_app_options options {};
        options.metafile = bittorrent_hybrid;

        SECTION("hybrid magnet url") {
            options.protocol = dt::protocol::hybrid;
            run_magnet_app(options);
            CHECK(buffer.str() == "magnet:?xt=urn:btih:8c9a2f583949c757c32e085413b581067eed47d0"
                                  "&xt=urn:btmh:1220d8dd32ac93357c368556af3ac1d95c9d76bd0dff6fa9833ecdac3d53134efabb"
                                  "&dn=bittorrent-v1-v2-hybrid-test\n");
        }
        SECTION("v1 magnet url") {
            options.protocol = dt::protocol::v1;
            run_magnet_app(options);
            CHECK(buffer.str() == "magnet:?xt=urn:btih:8c9a2f583949c757c32e085413b581067eed47d0"
                                  "&dn=bittorrent-v1-v2-hybrid-test\n");
        }
        SECTION("v2 magnet url") {
            options.protocol = dt::protocol::v2;
            run_magnet_app(options);
            CHECK(buffer.str() == "magnet:?xt=urn:btmh:1220d8dd32ac93357c368556af3ac1d95c9d76bd0dff6fa9833ecdac3d53134efabb"
                                  "&dn=bittorrent-v1-v2-hybrid-test\n");
        }
    }
}

