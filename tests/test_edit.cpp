#include <filesystem>
#include <ranges>

#include <experimental/source_location>

#include <catch2/catch.hpp>
#include <fmt/format.h>
#include <CLI/CLI.hpp>

#include <dottorrent/dht_node.hpp>
#include "edit.hpp"
#include "tracker_database.hpp"

#include "test_resources.hpp"

namespace dt = dottorrent;
namespace tt = torrenttools;
namespace fs = std::filesystem;
namespace rng = std::ranges;

#define PARSE_ARGS(cmd) \
try { app.parse(cmd); } catch (CLI::ParseError& e) { FAIL(e.what()); } \

#define PARSE_ARGS_THROWING(cmd) \
app.parse(cmd)

TEST_CASE("test edit app argument parsing")
{
    auto source = std::experimental::source_location::current();
    auto file = source.file_name();

    CLI::App app("test app", "torrenttools");
    auto edit_app = app.add_subcommand("edit",   "Create a new metafile");
    edit_app_options edit_options {};
    configure_edit_app(edit_app, edit_options);

    SECTION("test announce option")
    {
        std::string tracker1 = "https://test1.com/announce.php";
        std::string tracker2 = "https://test2.com/announce.php";
        std::string tracker3 = "https://test3.com/announce.php";
        std::string tracker_abbr1 = "HDB";

        SECTION("no --announce option given") {
            auto cmd = fmt::format("edit {}", file);
            PARSE_ARGS(cmd);

            CHECK_FALSE(edit_options.announce_list.has_value());
        }

        SECTION("single announce") {
            auto cmd = fmt::format("edit {} --announce {}", file, tracker1);
            PARSE_ARGS(cmd);

            CHECK(edit_options.announce_list.has_value());
            CHECK(edit_options.announce_list->size() == 1);
            CHECK(edit_options.announce_list->at(0) == std::vector{{tracker1}});
        }
        SECTION("multiple announces") {
            auto cmd = fmt::format("edit {} --announce {} {} {}", file, tracker1, tracker2, tracker3);
            PARSE_ARGS(cmd);

            CHECK(edit_options.announce_list.has_value());
            CHECK(edit_options.announce_list->size() == 3);
            CHECK(edit_options.announce_list->at(0) == std::vector{{tracker1}});
            CHECK(edit_options.announce_list->at(1) == std::vector{{tracker2}});
            CHECK(edit_options.announce_list->at(2) == std::vector{{tracker3}});
        }
        SECTION("multiple tiers") {
            auto cmd = fmt::format("edit {} --announce {} [{} {}]", file, tracker1, tracker2, tracker3);
            PARSE_ARGS(cmd);

            CHECK(edit_options.announce_list.has_value());
            CHECK(edit_options.announce_list->size() == 2);
            CHECK(edit_options.announce_list->at(0) == std::vector{{tracker1}});
            CHECK(edit_options.announce_list->at(1) == std::vector{{tracker2, tracker3}});
        }
    }

    SECTION("test dht nodes")
    {
        std::string node1 = "192.169.0.100:6464";
        std::string node2 = "8.8.8.8:80";
        std::string invalid_node = "21231kwfqfw";

        SECTION("no option given") {
            auto cmd = fmt::format("edit {}", file);
            PARSE_ARGS(cmd);

            CHECK_FALSE(edit_options.dht_nodes.has_value());
        }

        SECTION("single dht node") {
            auto cmd = fmt::format("edit {} --dht-node {}", file, node1);
            PARSE_ARGS(cmd);

            CHECK(edit_options.dht_nodes.has_value());
            CHECK(edit_options.dht_nodes->size() == 1);
            CHECK(edit_options.dht_nodes->at(0) == dt::dht_node{"192.169.0.100", 6464});
        }

        SECTION("multiple dht node") {
            auto cmd = fmt::format("edit {} --dht-node {} {}", file, node1, node2);
            PARSE_ARGS(cmd);

            CHECK(edit_options.dht_nodes.has_value());
            CHECK(edit_options.dht_nodes->size() == 2);
            CHECK(edit_options.dht_nodes->at(0) == dt::dht_node("192.169.0.100", 6464));
            CHECK(edit_options.dht_nodes->at(1) == dt::dht_node("8.8.8.8", 80));
        }

        SECTION("bad dht node") {
            auto cmd = fmt::format("edit {} --dht-node {}", file, invalid_node);
            CHECK_THROWS(PARSE_ARGS_THROWING(cmd));

            CHECK_FALSE(edit_options.dht_nodes.has_value());
        }
    }

    SECTION("test web seeds")
    {
        std::string seed1 = "https://web-seed-url.com/files1";
        std::string seed2 = "https://web-seed-url.com/files2";

        SECTION("no option given") {
            auto cmd = fmt::format("edit {}", file);
            PARSE_ARGS(cmd);

            CHECK_FALSE(edit_options.web_seeds.has_value());
        }
        SECTION("single web-seed ") {
            auto cmd = fmt::format("edit {} --web-seed {}", file, seed1);
            PARSE_ARGS(cmd);

            CHECK(edit_options.web_seeds.has_value());
            CHECK(edit_options.web_seeds->size() == 1);
            CHECK(edit_options.web_seeds->at(0) == seed1);
        }

        SECTION("multiple dht node") {
            auto cmd = fmt::format("edit {} --web-seed {} {}", file, seed1, seed2);
            PARSE_ARGS(cmd);

            CHECK(edit_options.web_seeds.has_value());
            CHECK(edit_options.web_seeds->size() == 2);
            CHECK(edit_options.web_seeds->at(0) == seed1);
            CHECK(edit_options.web_seeds->at(1) == seed2);
        }
    }
    SECTION("test comment")
    {
        SECTION("no option given") {
            auto cmd = fmt::format("edit {}", file);
            PARSE_ARGS(cmd);
            CHECK_FALSE(edit_options.comment.has_value());
        }
        SECTION("comment") {
            std::string comment = "Test comment";
            auto cmd = fmt::format("edit {} --comment \"{}\"", file, comment);
            PARSE_ARGS(cmd);

            CHECK(edit_options.comment.has_value());
            CHECK(edit_options.comment == comment);
        }
        SECTION("empty comment") {
            std::string comment = "Test comment";
            auto cmd = fmt::format("edit {} --comment \"{}\"", file, "");
            PARSE_ARGS(cmd);

            CHECK(edit_options.comment.has_value());
            CHECK(edit_options.comment->empty());
        }
    }

    SECTION("private flag") {
        SECTION("no option given") {
            auto cmd = fmt::format("edit {}", file);
            PARSE_ARGS(cmd);
            CHECK_FALSE(edit_options.is_private.has_value());
        }
        SECTION("explicit true") {
            auto cmd = fmt::format("edit {} --private on", file);
            PARSE_ARGS(cmd);
            CHECK(edit_options.is_private.has_value());
            CHECK(*edit_options.is_private);
        }
        SECTION("explicit off") {
            auto cmd = fmt::format("edit {} --private off", file);
            PARSE_ARGS(cmd);
            CHECK(edit_options.is_private.has_value());
            CHECK_FALSE(*edit_options.is_private);
        }
        SECTION("implicit on") {
            auto cmd = fmt::format("edit {} --private", file);
            PARSE_ARGS(cmd);
            CHECK(edit_options.is_private.has_value());
            CHECK(*edit_options.is_private);
        }
    }

    SECTION("source") {
        SECTION("no options given") {
            auto cmd = fmt::format("edit {}", file);
            PARSE_ARGS(cmd);
            CHECK_FALSE(edit_options.source.has_value());
        }
        SECTION("option given") {
            auto cmd = fmt::format("edit {} --source \"{}\"", file, "My Tracker");
            PARSE_ARGS(cmd);
            CHECK(edit_options.source.has_value());
            CHECK(*edit_options.source == "My Tracker");
        }
        SECTION("empty value") {
            auto cmd = fmt::format("edit {} --source \"{}\"", file, "");
            PARSE_ARGS(cmd);
            CHECK(edit_options.source.has_value());
            CHECK(edit_options.source->empty());
        }
    }

    SECTION("name") {
        SECTION("no option given") {
            auto cmd = fmt::format("edit {}", file);
            PARSE_ARGS(cmd);
            CHECK_FALSE(edit_options.name.has_value());
        }
        SECTION("option given") {
            auto cmd = fmt::format("edit {} --name \"{}\"", file, "renamed-file");
            PARSE_ARGS(cmd);
            CHECK(edit_options.name.has_value());
            CHECK(edit_options.name == "renamed-file");
        }
    }

    SECTION("creation date") {
        SECTION("no options given") {
            auto cmd = fmt::format("edit {}", file);
            PARSE_ARGS(cmd);
            CHECK_FALSE(edit_options.creation_date.has_value());
        }
        SECTION("POSIX time") {
            auto cmd = fmt::format("edit {} --creation-date {}", file, 1611339706);
            PARSE_ARGS(cmd);
            CHECK(edit_options.creation_date.has_value());
            CHECK(edit_options.creation_date == std::chrono::system_clock::time_point(std::chrono::seconds(1611339706)));
        }
        SECTION("POSIX time") {
            auto cmd = fmt::format("edit {} --creation-date {}", file, 1611339706);
            PARSE_ARGS(cmd);
            CHECK(edit_options.creation_date.has_value());
            CHECK(edit_options.creation_date == std::chrono::system_clock::time_point(std::chrono::seconds(1611339706)));
        }
        SECTION("ISO-8601 time - timezone offset") {
            auto cmd = fmt::format("edit {} --creation-date {}", file, "2021-01-22T19:21:46+0100");
            PARSE_ARGS(cmd);
            CHECK(edit_options.creation_date.has_value());
            CHECK(edit_options.creation_date == std::chrono::system_clock::time_point(std::chrono::seconds(1611339706)));
        }
        SECTION("empty value given") {
            auto cmd = fmt::format("edit {} --creation-date {}", file, "");
            PARSE_ARGS(cmd);
            CHECK(edit_options.creation_date.has_value());
            CHECK(edit_options.creation_date == std::chrono::system_clock::time_point{});
        }
    }

    SECTION("no-created-by") {
        SECTION("default") {
            auto cmd = fmt::format("edit {}", file);
            PARSE_ARGS(cmd);
            CHECK(edit_options.set_created_by);
        }
        SECTION("option given") {
            auto cmd = fmt::format("edit {} --no-created-by", file);
            PARSE_ARGS(cmd);
            CHECK_FALSE(edit_options.set_created_by);
        }
    }

    SECTION("no-creation-date") {
        SECTION("default") {
            auto cmd = fmt::format("edit {}", file);
            PARSE_ARGS(cmd);
            CHECK(edit_options.set_creation_date);
        }
        SECTION("option given") {
            auto cmd = fmt::format("edit {} --no-creation-date", file);
            PARSE_ARGS(cmd);
            CHECK_FALSE(edit_options.set_creation_date);
        }
    }

    SECTION("cross-seed") {
        SECTION("no flag given collection") {
            auto cmd = fmt::format("edit {}", file);
            PARSE_ARGS(cmd);
            CHECK_FALSE(edit_options.enable_cross_seeding);
        }
        SECTION("no flag given collection") {
            auto cmd = fmt::format("edit {} --cross-seed", file);
            PARSE_ARGS(cmd);
            CHECK(edit_options.enable_cross_seeding);
        }
    }
}


TEST_CASE("test edit announce")
{
    std::stringstream buffer {};
    temporary_directory tmp_dir {};
    main_app_options main_options{};

    fs::path output = fs::path(tmp_dir) / "test-edit-announce.torrent";

    edit_app_options options {
        .metafile = fedora_torrent,
        .destination = output,
    };

    SECTION("replace single tracker") {
        options.announce_list = std::vector<std::vector<std::string>>{
                {"https://test1.com/announce"}
        };
        options.list_mode = tt::list_edit_mode::replace;

        run_edit_app(main_options, options);
        auto m = dt::load_metafile(output);
        CHECK(dt::as_nested_vector(m.trackers()) == options.announce_list);
    }

    SECTION("list_mode - reset") {
        options.announce_list = std::vector<std::vector<std::string>> {
                {"https://test1.com/announce"},
                {"https://test2.com/announce"}
        };
        options.list_mode = tt::list_edit_mode::replace;

        run_edit_app(main_options, options);
        auto m = dt::load_metafile(output);
        CHECK(dt::as_nested_vector(m.trackers()) == options.announce_list);
    }
    SECTION("list_mode - append") {
        options.announce_list = std::vector<std::vector<std::string>> {
                {"https://test1.com/announce"},
                {"https://test2.com/announce"}
        };
        options.list_mode = tt::list_edit_mode::append;

        run_edit_app(main_options, options);
        auto m = dt::load_metafile(output);
        CHECK(m.trackers().at(0).url == "http://torrent.fedoraproject.org:6969/announce");
        CHECK(m.trackers().at(1).url == "https://test1.com/announce");
        CHECK(m.trackers().at(2).url == "https://test2.com/announce");
    }

    SECTION("list_mode - prepend") {
        options.announce_list = std::vector<std::vector<std::string>> {
                {"https://test1.com/announce"},
                {"https://test2.com/announce"}
        };
        options.list_mode = tt::list_edit_mode::prepend;

        run_edit_app(main_options, options);
        auto m = dt::load_metafile(output);
        CHECK(m.trackers().at(0).url == "https://test1.com/announce");
        CHECK(m.trackers().at(1).url == "https://test2.com/announce");
        CHECK(m.trackers().at(2).url == "http://torrent.fedoraproject.org:6969/announce");
    }
}

TEST_CASE("test edit announce-group")
{
    std::stringstream buffer{};
    temporary_directory tmp_dir{};
    main_app_options main_options{};

    fs::path output = fs::path(tmp_dir) / "test-edit-announce-group.torrent";

    main_options.config = fs::path(TEST_RESOURCES_DIR) / "config";
    main_options.tracker_db = fs::path(TEST_RESOURCES_DIR) / "config";

    edit_app_options options{
        .metafile = fedora_torrent,
        .destination = output,
    };

    SECTION("list mode: replace ") {
        options.announce_group_list = std::vector<std::string>{ "example-group-custom" };
        options.list_mode = tt::list_edit_mode::replace;

        run_edit_app(main_options, options);
        auto m = dt::load_metafile(output);
        CHECK(dt::as_nested_vector(m.trackers()) == std::vector<std::vector<std::string>>{
            {"udp://tracker.opentrackr.org:1337/announce"},
            {"udp://www.torrent.eu.org:451/announce"},
        });
    }

    SECTION("list mode: append ") {
        options.announce_group_list = std::vector<std::string>{ "example-group-custom" };
        options.list_mode = tt::list_edit_mode::append;

        run_edit_app(main_options, options);
        auto m = dt::load_metafile(output);
        CHECK(dt::as_nested_vector(m.trackers()) == std::vector<std::vector<std::string>>{
                {"http://torrent.fedoraproject.org:6969/announce"},
                {"udp://tracker.opentrackr.org:1337/announce"},
                {"udp://www.torrent.eu.org:451/announce"},
        });
    }

    SECTION("list mode: prepend ") {
        options.announce_group_list = std::vector<std::string>{ "example-group-custom" };
        options.list_mode = tt::list_edit_mode::prepend;

        run_edit_app(main_options, options);
        auto m = dt::load_metafile(output);
        CHECK(dt::as_nested_vector(m.trackers()) == std::vector<std::vector<std::string>>{
                {"udp://tracker.opentrackr.org:1337/announce"},
                {"udp://www.torrent.eu.org:451/announce"},
                {"http://torrent.fedoraproject.org:6969/announce"},
        });
    }
}


TEST_CASE("test edit web-seeds", "[edit]")
{
    std::stringstream buffer {};
    temporary_directory tmp_dir {};
    main_app_options main_options{};

    fs::path output = fs::path(tmp_dir) / "test-edit-webseeds.torrent";

    edit_app_options options {
        .metafile = web_seeds_torrent,
        .destination = output,
    };

    SECTION("list_mode - reset") {
        options.web_seeds = std::vector<std::string> {
                {"https://seed1.com/path"},
                {"https://seed2.com/path"}
        };
        options.list_mode = tt::list_edit_mode::replace;

        run_edit_app(main_options, options);
        auto m = dt::load_metafile(output);
        CHECK(m.web_seeds() == options.web_seeds);
    }
    SECTION("list_mode - append") {
        options.web_seeds = std::vector<std::string> {
                {"https://seed1.com/path"},
                {"https://seed2.com/path"}
        };
        options.list_mode = tt::list_edit_mode::append;

        run_edit_app(main_options, options);
        auto m = dt::load_metafile(output);
        CHECK(m.web_seeds().at(0) == "https://example.com/path:8666");
        CHECK(m.web_seeds().at(1) == "https://seed1.com/path");
        CHECK(m.web_seeds().at(2) == "https://seed2.com/path");
    }

    SECTION("list_mode - prepend") {
        options.web_seeds = std::vector<std::string> {
                {"https://seed1.com/path"},
                {"https://seed2.com/path"}
        };
        options.list_mode = tt::list_edit_mode::prepend;

        run_edit_app(main_options, options);
        auto m = dt::load_metafile(output);
        CHECK(m.web_seeds().at(0) == "https://seed1.com/path");
        CHECK(m.web_seeds().at(1) == "https://seed2.com/path");
        CHECK(m.web_seeds().at(2) == "https://example.com/path:8666");
    }
}


TEST_CASE("test edit http-seeds", "[edit]")
{
    std::stringstream buffer {};
    temporary_directory tmp_dir {};
    main_app_options main_options{};

    fs::path output = fs::path(tmp_dir) / "test-edit-http-seeds.torrent";

    edit_app_options options {
            .metafile = http_seeds_torrent,
            .destination = output,
    };

    SECTION("list_mode - reset") {
        options.http_seeds = std::vector<std::string> {
                {"https://seed1.com/path"},
                {"https://seed2.com/path"}
        };
        options.list_mode = tt::list_edit_mode::replace;

        run_edit_app(main_options, options);
        auto m = dt::load_metafile(output);
        CHECK(m.http_seeds() == options.http_seeds);
    }
    SECTION("list_mode - append") {
        options.http_seeds = std::vector<std::string> {
                {"https://seed1.com/path"},
                {"https://seed2.com/path"}
        };
        options.list_mode = tt::list_edit_mode::append;

        run_edit_app(main_options, options);
        auto m = dt::load_metafile(output);
        CHECK(m.http_seeds().at(0) == "http://test.url.com/httpseed");
        CHECK(m.http_seeds().at(1) == "https://seed1.com/path");
        CHECK(m.http_seeds().at(2) == "https://seed2.com/path");
    }

    SECTION("list_mode - prepend") {
        options.http_seeds = std::vector<std::string> {
                {"https://seed1.com/path"},
                {"https://seed2.com/path"}
        };
        options.list_mode = tt::list_edit_mode::prepend;

        run_edit_app(main_options, options);
        auto m = dt::load_metafile(output);
        CHECK(m.http_seeds().at(0) == "https://seed1.com/path");
        CHECK(m.http_seeds().at(1) == "https://seed2.com/path");
        CHECK(m.http_seeds().at(2) == "http://test.url.com/httpseed");
    }
}


TEST_CASE("test edit dht-nodes", "[edit]")
{
    std::stringstream buffer {};
    temporary_directory tmp_dir {};
    main_app_options main_options{};

    fs::path output = fs::path(tmp_dir) / "test-edit-dhtnodes.torrent";

    edit_app_options options {
            .metafile = web_seeds_torrent,
            .destination = output,
    };

    SECTION("list_mode - reset") {
        options.dht_nodes = std::vector<dt::dht_node> {
                dt::dht_node{"https://node1.com/path", 8668},
                dt::dht_node{"https://node2.com/path", 8888},
        };
        options.list_mode = tt::list_edit_mode::replace;

        run_edit_app(main_options, options);
        auto m = dt::load_metafile(output);
        CHECK(m.dht_nodes() == options.dht_nodes);
    }
    SECTION("list_mode - append") {
        options.metafile = dht_nodes_torrent;
        options.dht_nodes = std::vector<dt::dht_node> {
                dt::dht_node{"https://node1.com/path", 8887},
                dt::dht_node{"https://node2.com/path", 8888},
        };
        options.list_mode = tt::list_edit_mode::append;

        run_edit_app(main_options, options);
        auto m = dt::load_metafile(output);
        CHECK(m.dht_nodes().at(0) == dt::dht_node{"https://node.com/path", 8668});
        CHECK(m.dht_nodes().at(1) == dt::dht_node{"https://node1.com/path", 8887});
        CHECK(m.dht_nodes().at(2) == dt::dht_node{"https://node2.com/path", 8888});
    }

    SECTION("list_mode - prepend") {
        options.metafile = dht_nodes_torrent;
        options.dht_nodes = std::vector<dt::dht_node> {
                dt::dht_node{"https://node1.com/path", 8887},
                dt::dht_node{"https://node2.com/path", 8888},
        };
        options.list_mode = tt::list_edit_mode::prepend;

        run_edit_app(main_options, options);
        auto m = dt::load_metafile(output);
        CHECK(m.dht_nodes().at(0) == dt::dht_node{"https://node1.com/path", 8887});
        CHECK(m.dht_nodes().at(1) == dt::dht_node{"https://node2.com/path", 8888});
        CHECK(m.dht_nodes().at(2) == dt::dht_node{"https://node.com/path", 8668});
    }
}


TEST_CASE("test edit comment")
{
    std::stringstream buffer {};
    temporary_directory tmp_dir {};
    main_app_options main_options{};

    fs::path output = fs::path(tmp_dir) / "test-edit-comment.torrent";
    edit_app_options options {
            .metafile = web_seeds_torrent,
            .destination = output,
    };

    SECTION("replace comment") {
        options.comment = "new comment";
        run_edit_app(main_options, options);
        auto m = dt::load_metafile(output);
        CHECK(m.comment() == options.comment);
    }
    SECTION("delete comment") {
        options.comment = "";
        run_edit_app(main_options, options);
        auto m = dt::load_metafile(output);
        CHECK(m.comment() == "");
    }
}


TEST_CASE("test edit private")
{
    std::stringstream buffer {};
    temporary_directory tmp_dir {};
    main_app_options main_options{};

    fs::path output = fs::path(tmp_dir) / "test-edit-private.torrent";
    edit_app_options options {
            .destination = output,
    };

    SECTION("set off") {
        options.metafile = private_torrent;
        options.is_private = false;
        run_edit_app(main_options, options);
        auto m = dt::load_metafile(output);
        CHECK_FALSE(m.is_private());
    }
    SECTION("set on") {
        options.metafile = fedora_torrent;
        options.is_private = true;
        run_edit_app(main_options, options);
        auto m = dt::load_metafile(output);
        CHECK(m.is_private());
    }
}


TEST_CASE("test edit source")
{
    std::stringstream buffer {};
    temporary_directory tmp_dir {};
    main_app_options main_options{};

    fs::path output = fs::path(tmp_dir) / "test-edit-source.torrent";
    edit_app_options options {
            .metafile = fedora_torrent,
            .destination = output,
    };

    SECTION("edit source") {
        options.source = "torrenttools";
        run_edit_app(main_options, options);
        auto m = dt::load_metafile(output);
        CHECK(m.source() == options.source);
    }
    SECTION("remove source") {
        options.source = "";
        run_edit_app(main_options, options);
        auto m = dt::load_metafile(output);
        CHECK(m.source().empty());
    }
}

TEST_CASE("test edit name")
{
    std::stringstream buffer {};
    temporary_directory tmp_dir {};
    main_app_options main_options{};

    fs::path output = fs::path(tmp_dir) / "test-edit-name.torrent";
    edit_app_options options {
            .metafile = fedora_torrent,
            .destination = output,
    };

    SECTION("set name") {
        options.name = "new-name";
        run_edit_app(main_options, options);
        auto m = dt::load_metafile(output);
        CHECK(m.name() == "new-name");
    }
}


TEST_CASE("test edit creation-date")
{
    std::stringstream buffer {};
    temporary_directory tmp_dir {};
    main_app_options main_options{};

    fs::path output = fs::path(tmp_dir) / "test-edit-creation-date.torrent";
    edit_app_options options {
            .metafile = fedora_torrent,
            .destination = output,
    };

    SECTION("test update of creation date on edit") {
        auto old_creation_date = dt::load_metafile(fedora_torrent).creation_date();
        options.comment = "new comment";
        run_edit_app(main_options, options);
        auto m = dt::load_metafile(output);
        CHECK(m.creation_date() > old_creation_date);
    }
    SECTION("test no update of creation date with --no-creation-date") {
        auto old_creation_date = dt::load_metafile(fedora_torrent).creation_date();
        options.comment = "new comment";
        options.set_creation_date = false;
        run_edit_app(main_options, options);
        auto m = dt::load_metafile(output);
        CHECK(m.creation_date() == old_creation_date);
    }
    SECTION("test overriding creation date") {
        options.creation_date = std::chrono::system_clock::time_point(std::chrono::seconds(1611339706));
        run_edit_app(main_options, options);
        auto m = dt::load_metafile(output);
        CHECK(m.creation_date() == std::chrono::seconds(1611339706));
    }
}

TEST_CASE("test edit created-by")
{
    std::stringstream buffer {};
    temporary_directory tmp_dir {};
    main_app_options main_options{};

    fs::path output = fs::path(tmp_dir) / "test-edit-create-by.torrent";
    edit_app_options options {
            .metafile = fedora_torrent,
            .destination = output
    };

    SECTION("test update of created by on edit") {
        auto old_creation_date = dt::load_metafile(fedora_torrent).creation_date();
        options.comment = "new comment";
        run_edit_app(main_options, options);
        auto m = dt::load_metafile(output);
        CHECK(m.created_by() == CREATED_BY_STRING);
    }
    SECTION("test no update of creation date with --no-creation-date") {
        auto old_created_by = dt::load_metafile(fedora_torrent).created_by();
        options.comment = "new comment";
        options.set_created_by = false;
        run_edit_app(main_options, options);
        auto m = dt::load_metafile(output);
        CHECK(m.created_by() == old_created_by);
    }
    SECTION("test overriding created_by") {
        options.created_by = "me";
        run_edit_app(main_options, options);
        auto m = dt::load_metafile(output);
        CHECK(m.created_by() == options.created_by);
    }
}


TEST_CASE("test edit similar torrents", "[edit]")
{
    std::stringstream buffer {};
    temporary_directory tmp_dir {};
    main_app_options main_options{};

    fs::path output = fs::path(tmp_dir) / "test-edit-similar-v1.torrent";

    edit_app_options options {
            .metafile = similar_v1_torrent,
            .destination = output,
    };

    SECTION("list_mode - reset") {
        auto hash = dt::info_hash::from_hex(dt::protocol::v1, "7eda978ed7628595bb91c48b947f025bae78cb77");
        options.similar_torrents = std::vector<dt::info_hash>{hash};
        options.list_mode = tt::list_edit_mode::replace;

        run_edit_app(main_options, options);
        auto m = dt::load_metafile(output);
        CHECK(m.similar_torrents() == std::unordered_set{hash});
    }
    SECTION("list_mode - append/prepend") {
        auto m = dt::load_metafile(options.metafile);
        auto contained_similar_hash = *m.similar_torrents().begin();

        auto hash = dt::info_hash::from_hex(dt::protocol::v1, "7eda978ed7628595bb91c48b947f025bae78cb77");
        options.similar_torrents = std::vector<dt::info_hash>{hash};
        options.list_mode = tt::list_edit_mode::append;

        run_edit_app(main_options, options);
        m = dt::load_metafile(output);
        CHECK(m.similar_torrents() == std::unordered_set{contained_similar_hash, hash});
    }
}


TEST_CASE("test edit collections", "[edit]")
{
    std::stringstream buffer {};
    temporary_directory tmp_dir {};
    main_app_options main_options{};

    fs::path output = fs::path(tmp_dir) / "test-edit-collections.torrent";

    edit_app_options options {
            .metafile = collection_torrent,
            .destination = output,
    };

    SECTION("list_mode - reset") {
        options.collections = {"edit-test1"};
        options.list_mode = tt::list_edit_mode::replace;

        run_edit_app(main_options, options);
        auto m = dt::load_metafile(output);
        CHECK(m.collections() == std::unordered_set<std::string>{"edit-test1"});
    }
    SECTION("list_mode - append/prepend") {
        auto m = dt::load_metafile(options.metafile);
        auto original_collections = m.collections();

        options.collections = {"edit-test1"};
        options.list_mode = tt::list_edit_mode::append;

        run_edit_app(main_options, options);
        m = dt::load_metafile(output);
        std::unordered_set<std::string> joined {};
        rng::set_union(
                original_collections,
                std::unordered_set<std::string>({"edit-test1"}),
                std::inserter(joined, joined.begin()));
        CHECK(m.collections() == joined);
    }
}


TEST_CASE("test edit app: cross-seeding")
{
    std::stringstream buffer {};
    temporary_directory tmp_dir {};
    main_app_options main_options{};

    fs::path output = fs::path(tmp_dir) / "test-cross-seed-entry.torrent";

    edit_app_options options {
        .metafile = collection_torrent,
        .destination = output,
    };

    SECTION("include cross-seed entry") {
        options.enable_cross_seeding = true;

        run_edit_app(main_options, options);
        auto m = dt::load_metafile(output);
        CHECK(m.other_info_fields().contains("cross_seed_entry"));
    }
}