#include <experimental/source_location>
#include <sstream>
#include <ostream>


#include <catch2/catch.hpp>
#include <fmt/format.h>
#include <CLI/CLI.hpp>

#include "show.hpp"
#include "test_resources.hpp"

namespace dt = dottorrent;

#define PARSE_ARGS(cmd) \
try { app.parse(cmd); } catch (CLI::ParseError& e) { FAIL(e.what()); } \

#define PARSE_ARGS_THROWING(cmd) \
app.parse(cmd)


TEST_CASE("test show app argument parsing")
{
    auto source = std::experimental::source_location::current();
    auto file = source.file_name();

    CLI::App app("test app", "torrenttools");
    auto show_app = app.add_subcommand("show",   "Show specific fields of a metafile");
    show_app_options show_options {};
    configure_show_app(show_app, show_options);

    SECTION("announce --flat") {
        SECTION("on") {
            auto cmd = fmt::format("show announce --flat {}", file);
            PARSE_ARGS(cmd);
            CHECK(show_options.announce_flatten);
        }
        SECTION("off") {
            auto cmd = fmt::format("show announce {}", file);
            PARSE_ARGS(cmd);
            CHECK_FALSE(show_options.announce_flatten);
        }
    }

    SECTION("infohash --protocol") {
        SECTION("v1") {
            auto cmd = fmt::format("show infohash {} --protocol v1", file);
            PARSE_ARGS(cmd);
            CHECK(show_options.infohash_protocol == dt::protocol::v1);
        }
        SECTION("v2") {
            auto cmd = fmt::format("show infohash {} --protocol v2", file);
            PARSE_ARGS(cmd);
            CHECK(show_options.infohash_protocol == dt::protocol::v2);
        }
        SECTION("hybrid - default") {
            auto cmd = fmt::format("show infohash {}", file);
            PARSE_ARGS(cmd);
            CHECK(show_options.infohash_protocol == dt::protocol::hybrid);
        }
        SECTION("hybrid - explicit") {
            auto cmd = fmt::format("show infohash {} --protocol hybrid", file);
            PARSE_ARGS(cmd);
            CHECK(show_options.infohash_protocol == dt::protocol::hybrid);
        }
    }

    SECTION("infohash --truncate") {
        SECTION("on") {
            auto cmd = fmt::format("show infohash {} --truncate", file);
            PARSE_ARGS(cmd);
            CHECK(show_options.infohash_truncate);
        }
        SECTION("off") {
            auto cmd = fmt::format("show infohash {}", file);
            PARSE_ARGS(cmd);
            CHECK_FALSE(show_options.infohash_truncate);
        }
    }

    SECTION("piece size --human-readable") {
        SECTION("on") {
            auto cmd = fmt::format("show piece-size {} --human-readable", file);
            PARSE_ARGS(cmd);
            CHECK(show_options.piece_size_human_readable);
        }
        SECTION("off") {
            auto cmd = fmt::format("show piece-size {}", file);
            PARSE_ARGS(cmd);
            CHECK_FALSE(show_options.piece_size_human_readable);
        }
    }

    SECTION("file size --human-readable") {
        SECTION("on") {
            auto cmd = fmt::format("show size {} --human-readable", file);
            PARSE_ARGS(cmd);
            CHECK(show_options.file_size_human_readable);
        }
        SECTION("off") {
            auto cmd = fmt::format("show size {}", file);
            PARSE_ARGS(cmd);
            CHECK_FALSE(show_options.file_size_human_readable);
        }
    }

    SECTION("creation-date --iso") {
        SECTION("on") {
            auto cmd = fmt::format("show creation-date {} --iso", file);
            PARSE_ARGS(cmd);
            CHECK(show_options.creation_date_iso_format);
        }
        SECTION("off") {
            auto cmd = fmt::format("show creation-date {}", file);
            PARSE_ARGS(cmd);
            CHECK_FALSE(show_options.creation_date_iso_format);
        }
    }

    SECTION("files: --show-padding-files")
    {
        SECTION("on") {
            auto cmd = fmt::format("show files {} --show-padding-files", file);
            PARSE_ARGS(cmd);
            CHECK(show_options.show_padding_files);
        }
        SECTION("off") {
            auto cmd = fmt::format("show files {}", file);
            PARSE_ARGS(cmd);
            CHECK_FALSE(show_options.show_padding_files);
        }
    }
}


TEST_CASE("test show announce")
{
    std::stringstream buffer {};
    auto redirect_guard = cout_redirect(buffer.rdbuf());


    SECTION("ubuntu torrent") {
        show_app_options options { .metafile = ubuntu_torrent };

        SECTION("flat") {
            options.announce_flatten = true;
            run_show_announce_subapp(options);
            CHECK(buffer.str() == "https://torrent.ubuntu.com/announce\n"
                                  "https://ipv6.torrent.ubuntu.com/announce\n");
        }

        SECTION("per tier") {
            run_show_announce_subapp(options);
            CHECK(buffer.str() == "https://torrent.ubuntu.com/announce\n"
                                  "https://ipv6.torrent.ubuntu.com/announce\n");
        }
    }
    SECTION("CAMELYON torrent") {
        show_app_options options { .metafile = camelyon_torrent };

        SECTION("flat") {
            options.announce_flatten = true;
            run_show_announce_subapp(options);
            CHECK(buffer.str() ==   "https://academictorrents.com/announce.php\n"
                                    "udp://tracker.coppersurfer.tk:6969\n"
                                    "udp://tracker.opentrackr.org:1337/announce\n"
                                    "udp://tracker.openbittorrent.com:80/announce\n");
        }
    }
}


TEST_CASE("test show protocol")
{
    std::stringstream buffer {};
    auto redirect_guard = cout_redirect(buffer.rdbuf());

    show_app_options options {};

    SECTION("hybrid") {
        options.metafile = bittorrent_hybrid;
        run_show_protocol_subapp(options);
        CHECK(buffer.str() == "hybrid\n");
    }
    SECTION("v2") {
        options.metafile = bittorrent_v2;
        run_show_protocol_subapp(options);
        CHECK(buffer.str() == "2\n");
    }
    SECTION("v1") {
        options.metafile = fedora_torrent;
        run_show_protocol_subapp(options);
        CHECK(buffer.str() == "1\n");
    }
}

TEST_CASE("test show infohash")
{
    std::stringstream buffer {};
    auto redirect_guard = cout_redirect(buffer.rdbuf());

    show_app_options options {};

    SECTION("hybrid") {
        options.metafile = bittorrent_hybrid;

        SECTION("protocol - hybrid") {
        options.infohash_protocol = dt::protocol::hybrid;
        run_show_infohash_subapp(options);
            CHECK(buffer.str() == "8c9a2f583949c757c32e085413b581067eed47d0\n"
                                  "d8dd32ac93357c368556af3ac1d95c9d76bd0dff6fa9833ecdac3d53134efabb\n");
        }
        SECTION("protocol - v1") {
            options.infohash_protocol = dt::protocol::v1;
            run_show_infohash_subapp(options);
            CHECK(buffer.str() == "8c9a2f583949c757c32e085413b581067eed47d0\n");
        }
        SECTION("protocol - v2") {
            options.infohash_protocol = dt::protocol::v2;
            run_show_infohash_subapp(options);
            CHECK(buffer.str() == "d8dd32ac93357c368556af3ac1d95c9d76bd0dff6fa9833ecdac3d53134efabb\n");
        }
        SECTION("protocol - v2 truncated") {
            options.infohash_truncate = true;
            options.infohash_protocol = dt::protocol::v2;
            run_show_infohash_subapp(options);
            CHECK(buffer.str() == "d8dd32ac93357c368556af3ac1d95c9d76bd0dff\n");
        }
    }
    SECTION("v1") {
        options.metafile = fedora_torrent;
        run_show_infohash_subapp(options);
        CHECK(buffer.str() == "aec2e48d6ece459f8358aad4889dc83046746b0b\n");
    }
    SECTION("v2") {
        options.metafile = bittorrent_v2;

        SECTION("full") {
            run_show_infohash_subapp(options);
            CHECK(buffer.str() == "caf1e1c30e81cb361b9ee167c4aa64228a7fa4fa9f6105232b28ad099f3a302e\n");
        }
        SECTION("truncated") {
            options.infohash_truncate = true;
            run_show_infohash_subapp(options);
            CHECK(buffer.str() == "caf1e1c30e81cb361b9ee167c4aa64228a7fa4fa\n");
        }
    }
}

TEST_CASE("test show piece-size")
{
    std::stringstream buffer {};
    auto redirect_guard = cout_redirect(buffer.rdbuf());

    show_app_options options{
        .metafile = fedora_torrent
    };

    SECTION("in bytes") {
        run_show_piece_size_subapp(options);
        CHECK(buffer.str() == "262144\n");
    }

    SECTION("in human readable format") {
        options.piece_size_human_readable = true;
        run_show_piece_size_subapp(options);
        CHECK(buffer.str() == "256 KiB\n");
    }
}


TEST_CASE("test show size")
{
    std::stringstream buffer {};
    auto redirect_guard = cout_redirect(buffer.rdbuf());

    show_app_options options{
            .metafile = fedora_torrent
    };

    SECTION("in bytes") {
        run_show_file_size_subapp(options);
        CHECK(buffer.str() == "1934755007\n");
    }

    SECTION("in human readable format") {
        options.file_size_human_readable = true;
        run_show_file_size_subapp(options);
        CHECK(buffer.str() == "1.80 GiB\n");
    }
}

TEST_CASE("test show created-by")
{
    std::stringstream buffer {};
    auto redirect_guard = cout_redirect(buffer.rdbuf());

    show_app_options options{ .metafile = camelyon_torrent };
    run_show_created_by_subapp(options);
    CHECK(buffer.str() == "Transmission/2.92 (14714)\n");
}


TEST_CASE("test show creation date")
{
    std::stringstream buffer {};
    auto redirect_guard = cout_redirect(buffer.rdbuf());
    show_app_options options{ .metafile = fedora_torrent };

    SECTION("POSIX timestamp") {
        run_show_creation_date_subapp(options);
        CHECK(buffer.str() == "1556547852\n");
    }

    SECTION("ISO 8006 timestamp") {
        options.creation_date_iso_format = true;
        run_show_creation_date_subapp(options);
        CHECK(buffer.str() == "2019-04-29T14:24:12Z\n");
    }
}

TEST_CASE("test show private")
{
    std::stringstream buffer {};
    auto redirect_guard = cout_redirect(buffer.rdbuf());
    show_app_options options {};

    SECTION("false") {
        options.metafile = fedora_torrent;
        run_show_private_subapp(options);
        CHECK(buffer.str() == "0\n");
    }

    SECTION("true") {
        options.metafile = private_torrent;
        run_show_private_subapp(options);
        CHECK(buffer.str() == "1\n");
    }
}

TEST_CASE("test show name")
{
    std::stringstream buffer {};
    auto redirect_guard = cout_redirect(buffer.rdbuf());
    show_app_options options {};
    options.metafile = fedora_torrent;
    run_show_name_subapp(options);
    CHECK(buffer.str() == "Fedora-Workstation-Live-x86_64-30\n");
}

TEST_CASE("test show comment")
{
    std::stringstream buffer {};
    auto redirect_guard = cout_redirect(buffer.rdbuf());
    show_app_options options {};
    options.metafile = camelyon_torrent;
    run_show_comment_subapp(options);
    CHECK(buffer.str() == "\n");
}

TEST_CASE("test show source")
{
    std::stringstream buffer {};
    auto redirect_guard = cout_redirect(buffer.rdbuf());
    show_app_options options {};
    options.metafile = private_torrent;
    run_show_source_subapp(options);
    CHECK(buffer.str() == "test\n");
}


TEST_CASE("test show query")
{
    std::stringstream buffer {};
    auto redirect_guard = cout_redirect(buffer.rdbuf());
    show_app_options options {};
    options.metafile = private_torrent;
    run_show_source_subapp(options);
    CHECK(buffer.str() == "test\n");
}

TEST_CASE("test show files")
{
    std::stringstream buffer {};
    auto redirect_guard = cout_redirect(buffer.rdbuf());
    show_app_options options {
        .metafile = bittorrent_hybrid
    };

    SECTION("files only") {
        std::string expected = (
R"(Darkroom (Stellar, 1994, Amiga ECS) HQ.mp4
Spaceballs-StateOfTheArt.avi
cncd_fairlight-ceasefire_(all_falls_down)-1080p.mp4
eld-dust.mkv
fairlight_cncd-agenda_circling_forth-1080p30lq.mp4
meet the deadline - Still _ Evoke 2014.mp4
readme.txt
tbl-goa.avi
tbl-tint.mpg
)");

        run_show_files_subapp(options);
        CHECK(buffer.str() == expected);
    }

    SECTION("files and padding files only") {
        options.show_padding_files = true;
        std::string expected = (
R"(Darkroom (Stellar, 1994, Amiga ECS) HQ.mp4
.pad/280339
Spaceballs-StateOfTheArt.avi
.pad/464896
cncd_fairlight-ceasefire_(all_falls_down)-1080p.mp4
.pad/129434
eld-dust.mkv
.pad/227380
fairlight_cncd-agenda_circling_forth-1080p30lq.mp4
.pad/507162
meet the deadline - Still _ Evoke 2014.mp4
.pad/510995
readme.txt
.pad/524227
tbl-goa.avi
.pad/442368
tbl-tint.mpg
)");

        run_show_files_subapp(options);
        CHECK(buffer.str() == expected);
    }

    SECTION("with prefix") {
        options.files_prefix = "/path/to/torrent/data";
        std::string expected = (
R"(/path/to/torrent/data/Darkroom (Stellar, 1994, Amiga ECS) HQ.mp4
/path/to/torrent/data/Spaceballs-StateOfTheArt.avi
/path/to/torrent/data/cncd_fairlight-ceasefire_(all_falls_down)-1080p.mp4
/path/to/torrent/data/eld-dust.mkv
/path/to/torrent/data/fairlight_cncd-agenda_circling_forth-1080p30lq.mp4
/path/to/torrent/data/meet the deadline - Still _ Evoke 2014.mp4
/path/to/torrent/data/readme.txt
/path/to/torrent/data/tbl-goa.avi
/path/to/torrent/data/tbl-tint.mpg
)");
        run_show_files_subapp(options);
        CHECK(buffer.str() == expected);
    }
}