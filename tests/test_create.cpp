
#include <experimental/source_location>

#include <catch2/catch.hpp>
#include <fmt/format.h>
#include <CLI/CLI.hpp>

#include <dottorrent/dht_node.hpp>
#include "create.hpp"
#include "tracker_database.hpp"
#include "test_resources.hpp"

namespace dt = dottorrent;
namespace tt = torrenttools;

#define PARSE_ARGS(cmd) \
try { app.parse(cmd); } catch (CLI::ParseError& e) { FAIL(e.what()); } \

#define PARSE_ARGS_THROWING(cmd) \
app.parse(cmd)

TEST_CASE("test create app argument parsing")
{
    const auto* tracker_database = tt::load_tracker_database();

    auto source = std::experimental::source_location::current();
    auto file = source.file_name();

    CLI::App app("test app", "torrenttools");
    auto create_app = app.add_subcommand("create",   "Create a new metafile");
    create_app_options create_options {};
    configure_create_app(create_app, create_options);

    SECTION("test announce option")
    {
        std::string tracker1 = "https://test1.com/announce.php";
        std::string tracker2 = "https://test2.com/announce.php";
        std::string tracker3 = "https://test3.com/announce.php";
        std::string tracker_abbr1 = "HDB";
        std::string tracker_abbrv1_url = tracker_database->at(tracker_abbr1).announce_url;

        SECTION("single announce") {
            auto cmd = fmt::format("create {} --announce {}", file, tracker1);

            PARSE_ARGS(cmd);

            CHECK(create_options.announce_list.size() == 1);
            CHECK(create_options.announce_list.at(0) == std::vector{{tracker1}});
        }
        SECTION("multiple announces") {
            auto cmd = fmt::format("create {} --announce {} {} {}", file, tracker1, tracker2, tracker3);

            PARSE_ARGS(cmd);

            CHECK(create_options.announce_list.size() == 3);
            CHECK(create_options.announce_list.at(0) == std::vector{{tracker1}});
            CHECK(create_options.announce_list.at(1) == std::vector{{tracker2}});
            CHECK(create_options.announce_list.at(2) == std::vector{{tracker3}});
        }
        SECTION("multiple tiers") {
            auto cmd = fmt::format("create {} --announce {} [{} {}]", file, tracker1, tracker2, tracker3);

            PARSE_ARGS(cmd);

            CHECK(create_options.announce_list.size() == 2);
            CHECK(create_options.announce_list.at(0) == std::vector{{tracker1}});
            CHECK(create_options.announce_list.at(1) == std::vector{{tracker2, tracker3}});
        }
    }

    SECTION("test dht nodes")
    {
        std::string node1 = "192.169.0.100:6464";
        std::string node2 = "8.8.8.8:80";
        std::string invalid_node = "21231kwfqfw";

        SECTION("single dht node") {
            auto cmd = fmt::format("create {} --dht-node {}", file, node1);
            PARSE_ARGS(cmd);

            CHECK(create_options.dht_nodes.size() == 1);
            CHECK(create_options.dht_nodes.at(0) == dt::dht_node{.url="192.169.0.100", .port=6464});
        }

        SECTION("multiple dht node") {
            auto cmd = fmt::format("create {} --dht-node {} {}", file, node1, node2);
            PARSE_ARGS(cmd);

            CHECK(create_options.dht_nodes.size() == 2);
            CHECK(create_options.dht_nodes.at(0) == dt::dht_node{.url="192.169.0.100", .port=6464});
            CHECK(create_options.dht_nodes.at(1) == dt::dht_node{.url="8.8.8.8", .port=80});
        }

        SECTION("bad dht node") {
            auto cmd = fmt::format("create {} --dht-node {}", file, invalid_node);
            CHECK_THROWS(PARSE_ARGS_THROWING(cmd));

            CHECK(create_options.dht_nodes.size() == 0);
        }
    }

    SECTION("test web seeds")
    {
        std::string seed1 = "https://web-seed-url.com/files1";
        std::string seed2 = "https://web-seed-url.com/files2";


        SECTION("single web-seed ") {
            auto cmd = fmt::format("create {} --web-seed {}", file, seed1);
            PARSE_ARGS(cmd);

            CHECK(create_options.web_seeds.size() == 1);
            CHECK(create_options.web_seeds.at(0) == seed1);
        }

        SECTION("multiple dht node") {
            auto cmd = fmt::format("create {} --web-seed {} {}", file, seed1, seed2);
            PARSE_ARGS(cmd);

            CHECK(create_options.web_seeds.size() == 2);
            CHECK(create_options.web_seeds.at(0) == seed1);
            CHECK(create_options.web_seeds.at(1) == seed2);
        }
    }
    SECTION("test comment")
    {
        SECTION("set comment") {
            std::string comment = "Test comment";
            auto cmd = fmt::format("create {} --comment \"{}\"", file, comment);
            PARSE_ARGS(cmd);

            CHECK(create_options.comment == comment);
        }
        SECTION("no comment given") {
            auto cmd = fmt::format("create {}", file);
            PARSE_ARGS(cmd);
            CHECK(create_options.comment == std::nullopt);
        }
    }

    SECTION("private flag") {
        SECTION("no option given") {
            auto cmd = fmt::format("create {}", file);
            PARSE_ARGS(cmd);
            CHECK_FALSE(create_options.is_private.has_value());
        }
        SECTION("explicit true") {
            auto cmd = fmt::format("create {} --private on", file);
            PARSE_ARGS(cmd);
            CHECK(create_options.is_private.has_value());
            CHECK(*create_options.is_private);
        }
        SECTION("explicit off") {
            auto cmd = fmt::format("create {} --private off", file);
            PARSE_ARGS(cmd);
            CHECK(create_options.is_private.has_value());
            CHECK_FALSE(*create_options.is_private);
        }
        SECTION("implicit on") {
            auto cmd = fmt::format("create {} --private", file);
            PARSE_ARGS(cmd);
            CHECK(create_options.is_private.has_value());
            CHECK(*create_options.is_private);
        }
    }

    SECTION("piece size") {
        SECTION("as power of two") {
            auto cmd = fmt::format("create {} --piece-size {}", file, 20);
            PARSE_ARGS(cmd);
            CHECK(create_options.piece_size.has_value());
            CHECK(*create_options.piece_size == 1 << 20);
        }

        SECTION("in KiB") {
            auto cmd = fmt::format("create {} --piece-size {}", file, "1024K");
            PARSE_ARGS(cmd);
            CHECK(create_options.piece_size.has_value());
            CHECK(*create_options.piece_size == 1 << 20);
        }

        SECTION("in MiB") {
            auto cmd = fmt::format("create {} --piece-size \"{}\"", file, "1M");
            PARSE_ARGS(cmd);
            CHECK(create_options.piece_size.has_value());
            CHECK(*create_options.piece_size == 1 << 20);
        }

        SECTION("space between unit and value") {
            auto cmd = fmt::format("create {} --piece-size \"{}\"", file, "1 MiB");
            PARSE_ARGS(cmd);
            CHECK(create_options.piece_size.has_value());
            CHECK(*create_options.piece_size == 1 << 20);
        }

        SECTION("explicit auto piece size") {
            auto cmd = fmt::format("create {} --piece-size \"{}\"", file, "auto");
            PARSE_ARGS(cmd);
            CHECK_FALSE(create_options.piece_size.has_value());
        }
    }

    SECTION("threads") {
        SECTION("default") {
            auto cmd = fmt::format("create {}", file);
            PARSE_ARGS(cmd);
            CHECK(create_options.threads == 2);
        }
        SECTION("option given") {
            auto cmd = fmt::format("create {} --threads 4", file);
            PARSE_ARGS(cmd);
            CHECK(create_options.threads == 4);
        }
    }

    SECTION("source") {
        SECTION("default") {
            auto cmd = fmt::format("create {}", file);
            PARSE_ARGS(cmd);
            CHECK_FALSE(create_options.source.has_value());
        }
        SECTION("option given") {
            auto cmd = fmt::format("create {} --source \"{}\"", file, "My Tracker");
            PARSE_ARGS(cmd);
            CHECK(create_options.source.has_value());
            CHECK(*create_options.source == "My Tracker");
        }
    }

    SECTION("name") {
        SECTION("default") {
            auto cmd = fmt::format("create {}", file);
            PARSE_ARGS(cmd);
            CHECK_FALSE(create_options.name.has_value());
        }
        SECTION("option given") {
            auto cmd = fmt::format("create {} --name \"{}\"", file, "renamed-file");
            PARSE_ARGS(cmd);
            CHECK(create_options.name.has_value());
            CHECK(create_options.name == "renamed-file");
        }
    }

    SECTION("checksums") {
        SECTION("default") {
            auto cmd = fmt::format("create {}", file);
            PARSE_ARGS(cmd);
            CHECK(create_options.checksums.empty());
        }
        SECTION("single checksum given") {
            auto cmd = fmt::format("create {} --checksum \"{}\"", file, "sha512");
            PARSE_ARGS(cmd);
            CHECK(create_options.checksums.size() == 1);
            CHECK(create_options.checksums.find(dt::hash_function::sha512) != create_options.checksums.end());
        }
        SECTION("multiple checksum given") {
            auto cmd = fmt::format(R"(create {} --checksum "{}" "{}")", file, "sha512", "blake2b_512");
            PARSE_ARGS(cmd);
            CHECK(create_options.checksums.size() == 2);
            CHECK(create_options.checksums.find(dt::hash_function::sha512) != create_options.checksums.end());
            CHECK(create_options.checksums.find(dt::hash_function::blake2b_512) != create_options.checksums.end());
        }
        SECTION("invalid checksum given") {
            auto cmd = fmt::format(R"(create {} --checksum "{}")", file, "blabla");
            CHECK_THROWS(PARSE_ARGS_THROWING(cmd));
        }
    }

    SECTION("no-created-by") {
        SECTION("default") {
            auto cmd = fmt::format("create {}", file);
            PARSE_ARGS(cmd);
            CHECK(create_options.set_created_by);
        }
        SECTION("option given") {
            auto cmd = fmt::format("create {} --no-created-by", file);
            PARSE_ARGS(cmd);
            CHECK_FALSE(create_options.set_created_by);
        }
    }

    SECTION("no-creation-date") {
        SECTION("default") {
            auto cmd = fmt::format("create {}", file);
            PARSE_ARGS(cmd);
            CHECK(create_options.set_creation_date);
        }
        SECTION("option given") {
            auto cmd = fmt::format("create {} --no-creation-date", file);
            PARSE_ARGS(cmd);
            CHECK_FALSE(create_options.set_creation_date);
        }
    }

    SECTION("creation date") {
        SECTION("default") {
            auto cmd = fmt::format("create {}", file);
            PARSE_ARGS(cmd);
            CHECK_FALSE(create_options.creation_date.has_value());
        }
        SECTION("POSIX time") {
            auto cmd = fmt::format("create {} --creation-date {}", file, 1611339706);
            PARSE_ARGS(cmd);
            CHECK(create_options.creation_date.has_value());
            CHECK(create_options.creation_date == std::chrono::system_clock::time_point(std::chrono::seconds(1611339706)));
        }
        SECTION("POSIX time") {
            auto cmd = fmt::format("create {} --creation-date {}", file, 1611339706);
            PARSE_ARGS(cmd);
            CHECK(create_options.creation_date.has_value());
            CHECK(create_options.creation_date == std::chrono::system_clock::time_point(std::chrono::seconds(1611339706)));
        }
        SECTION("POSIX time - no seperators") {
            auto cmd = fmt::format("create {} --creation-date {}", file, "2021-01-22T18:21:46Z+0100");
            PARSE_ARGS(cmd);
            CHECK(create_options.creation_date.has_value());
            CHECK(create_options.creation_date == std::chrono::system_clock::time_point(std::chrono::seconds(1611339706)));
        }
    }

    SECTION("include") {
        SECTION("default") {
            auto cmd = fmt::format("create {}", file);
            PARSE_ARGS(cmd);
            CHECK(create_options.include_patterns.empty());
        }
        SECTION("single value given") {
            auto cmd = fmt::format("create {} --include \"{}\"", file, ".*\\.git.*");
            PARSE_ARGS(cmd);
            CHECK(create_options.include_patterns.size() == 1);
            CHECK(create_options.include_patterns.at(0) == ".*\\.git.*");
        }
        SECTION("multiple values given") {
            auto cmd = fmt::format(R"(create {} --include "{}" "{}" --include-hidden)", file, "*.\\.git.*", ".*test.*" );
            PARSE_ARGS(cmd);
            CHECK(create_options.include_patterns.size() == 2);
            CHECK(create_options.include_patterns.at(0) == "*.\\.git.*");
            CHECK(create_options.include_patterns.at(1) == ".*test.*");
        }
    }

    SECTION("exclude") {
        SECTION("default") {
            auto cmd = fmt::format("create {}", file);
            PARSE_ARGS(cmd);
            CHECK(create_options.include_patterns.empty());
        }
        SECTION("single value given") {
            auto cmd = fmt::format("create {} --exclude \"{}\"", file, "*.git.*");
            PARSE_ARGS(cmd);
            CHECK(create_options.exclude_patterns.size() == 1);
            CHECK(create_options.exclude_patterns.at(0) == "*.git.*");
        }
        SECTION("multiple values given") {
            auto cmd = fmt::format(R"(create {} --exclude "{}" "{}" --include-hidden)", file, "*.\\.git.*", ".*test.*" );
            PARSE_ARGS(cmd);
            CHECK(create_options.exclude_patterns.size() == 2);
            CHECK(create_options.exclude_patterns.at(0) ==  "*.\\.git.*");
            CHECK(create_options.exclude_patterns.at(1) == ".*test.*");
        }
    }

    SECTION("include-hidden") {
        SECTION("default") {
            auto cmd = fmt::format("create {}", file);
            PARSE_ARGS(cmd);
            CHECK_FALSE(create_options.include_hidden_files);
        }
        SECTION("option given") {
            auto cmd = fmt::format("create {} --include-hidden", file);
            PARSE_ARGS(cmd);
            CHECK(create_options.include_hidden_files);
        }
    }

    SECTION("io-block-size") {
        SECTION("as power of two") {
            auto cmd = fmt::format("create {} --io-block-size {}", file, 20);
            PARSE_ARGS(cmd);
            CHECK(create_options.io_block_size.has_value());
            CHECK(*create_options.io_block_size == 1 << 20);
        }

        SECTION("in KiB") {
            auto cmd = fmt::format("create {} --io-block-size {}", file, "1024K");
            PARSE_ARGS(cmd);
            CHECK(create_options.io_block_size.has_value());
            CHECK(*create_options.io_block_size == 1 << 20);
        }

        SECTION("in MiB") {
            auto cmd = fmt::format("create {} --io-block-size \"{}\"", file, "1M");
            PARSE_ARGS(cmd);
            CHECK(create_options.io_block_size.has_value());
            CHECK(*create_options.io_block_size == 1 << 20);
        }
        SECTION("as power of two") {
            auto cmd = fmt::format("create {} --io-block-size {}", file, 20);
            PARSE_ARGS(cmd);
            CHECK(create_options.io_block_size.has_value());
            CHECK(*create_options.io_block_size == 1 << 20);
        }

        SECTION("in KiB") {
            auto cmd = fmt::format("create {} --io-block-size {}", file, "1024K");
            PARSE_ARGS(cmd);
            CHECK(create_options.io_block_size.has_value());
            CHECK(*create_options.io_block_size == 1 << 20);
        }

        SECTION("in MiB") {
            auto cmd = fmt::format("create {} --io-block-size \"{}\"", file, "1M");
            PARSE_ARGS(cmd);
            CHECK(create_options.io_block_size.has_value());
            CHECK(*create_options.io_block_size == 1 << 20);
        }

        SECTION("space between unit and value") {
            auto cmd = fmt::format("create {} --io-block-size \"{}\"", file, "1 MiB");
            PARSE_ARGS(cmd);
            CHECK(create_options.io_block_size.has_value());
            CHECK(*create_options.io_block_size == 1 << 20);
        }

        SECTION("explicit auto piece size") {
            auto cmd = fmt::format("create {} --io-block-size \"{}\"", file, "auto");
            PARSE_ARGS(cmd);
            CHECK_FALSE(create_options.io_block_size.has_value());
        }
        SECTION("space between unit and value") {
            auto cmd = fmt::format("create {} --io-block-size \"{}\"", file, "1 MiB");
            PARSE_ARGS(cmd);
            CHECK(create_options.io_block_size.has_value());
            CHECK(*create_options.io_block_size == 1 << 20);
        }

        SECTION("explicit auto piece size") {
            auto cmd = fmt::format("create {} --io-block-size \"{}\"", file, "auto");
            PARSE_ARGS(cmd);
            CHECK_FALSE(create_options.io_block_size.has_value());
        }
    }
    SECTION("output") {
        SECTION("default") {
            auto cmd = fmt::format("create {}", file);
            PARSE_ARGS(cmd);
            CHECK_FALSE(create_options.destination);
        }
        SECTION("full path") {
            auto output =  fs::path("/home/test/output.torrent");
            auto cmd = fmt::format("create {} --output {}", file, output.string());
            PARSE_ARGS(cmd);
            CHECK(create_options.destination == "/home/test/output.torrent");
        }
        SECTION("Directory with trailing slash") {
            auto output = fs::path("/home/test/Downloads/");
            auto cmd = fmt::format("create {} --output {}", file, output.string());
            PARSE_ARGS(cmd);
            CHECK(create_options.destination == output);
        }
        SECTION("Existing directory -> append trailing slash") {
            temporary_directory tmp_dir {};

            auto cmd = fmt::format("create {} --output {}", file,  tmp_dir.path());
            PARSE_ARGS(cmd);
            CHECK(create_options.destination == tmp_dir.path() / "");
        }
    }
}


TEST_CASE("test create app: target")
{
    temporary_directory tmp_dir {};

    SECTION("target is file") {
        fs::path target = camelyon_torrent;
        fs::path output = fs::path(tmp_dir) / "test-file-target.torrent";

        create_app_options options { .target = target, .destination = output, };
        run_create_app(options);
        auto m = dt::load_metafile(output);
        const auto& storage = m.storage();
        CHECK(storage.size() == 1);
        CHECK(storage.at(0).path().filename() == camelyon_torrent.filename());
    }
    SECTION("target is directory") {
        fs::path target = TEST_RESOURCES_DIR;
        fs::path output = fs::path(tmp_dir) / "test-directory-target.torrent";

        create_app_options options { .target = target, .destination = output, };
        run_create_app(options);
        auto m = dt::load_metafile(output);
        const auto& storage = m.storage();
        CHECK(m.name() == target.filename());
    }
}

TEST_CASE("test create app: announce-url")
{
    temporary_directory tmp_dir {};
    const auto* db = torrenttools::load_tracker_database();
    const auto* config = torrenttools::load_config();

    fs::path target = fs::path(TEST_RESOURCES_DIR);

    create_app_options options {
        .target = target,
        .destination = tmp_dir.path()
    };

    SECTION("tracker abbreviation") {
        fs::path expected_destination = tmp_dir.path() / fmt::format("[HDB]{}.torrent", target.filename().string());

        const auto& db_entry = db->at("hdb");
        options.announce_list = {{"hdb"}};
        run_create_app(options);

        REQUIRE(fs::exists(expected_destination));
        auto m = dt::load_metafile(expected_destination);

        CHECK(m.is_private() == db_entry.is_private);
        CHECK(m.source() == db_entry.name);
        CHECK(m.trackers().at(0).url == db_entry.substitute_parameters(*config));
    }
}

TEST_CASE("test create app: source tag")
{
    temporary_directory tmp_dir {};

    fs::path output = fs::path(tmp_dir) / "test-create-source.torrent";

    create_app_options create_options {
        .target = fs::path(TEST_DIR) / "resources",
        .destination = output,
        .source = "test"
    };
    run_create_app(create_options);
    auto m = dt::load_metafile(output);

    CHECK(m.source() == create_options.source);
}


TEST_CASE("test create app: private flag")
{
    temporary_directory tmp_dir {};

    fs::path output = fs::path(tmp_dir) / "test-create-private.torrent";

    create_app_options create_options {
            .target = fs::path(TEST_DIR) / "resources",
            .destination = output,
    };

    SECTION("true") {
        auto options = create_options;
        options.is_private = true;
        run_create_app(options);
        auto m = dt::load_metafile(output);
        CHECK(m.is_private() == options.is_private);
    }
    SECTION("false") {
        auto options = create_options;
        options.is_private = true;
        run_create_app(options);
        auto m = dt::load_metafile(output);
        CHECK(m.is_private() == options.is_private);
    }
}


TEST_CASE("test create app: creation-date")
{
    std::stringstream buffer {};
    temporary_directory tmp_dir {};

    fs::path output = fs::path(tmp_dir) / "test-edit-creation-date.torrent";
    create_app_options create_options {
            .target = fs::path(TEST_DIR) / "resources",
            .destination = output,
    };

    SECTION("--no-creation-date") {
        auto options = create_options;
        options.set_creation_date = false;
        run_create_app(options);
        auto m = dt::load_metafile(output);
        CHECK(m.creation_date() == std::chrono::seconds(0));
    }
    SECTION("overriding creation date") {
        auto options = create_options;
        options.creation_date = std::chrono::system_clock::time_point(std::chrono::seconds(1611339706));
        run_create_app(options);
        auto m = dt::load_metafile(output);
        CHECK(m.creation_date() == std::chrono::seconds(1611339706));
    }
}

TEST_CASE("test create app: created-by")
{
    std::stringstream buffer {};
    temporary_directory tmp_dir {};

    fs::path output = fs::path(tmp_dir) / "test-create-created-by.torrent";
    create_app_options create_options {
            .target = fs::path(TEST_DIR) / "resources",
            .destination = output,
    };

    SECTION("--no-created-by") {
        auto options = create_options;
        options.set_created_by = false;
        run_create_app(options);
        auto m = dt::load_metafile(output);
        CHECK(m.created_by().empty());
    }
    SECTION("overriding created-by") {
        auto options = create_options;
        options.created_by = "me";
        run_create_app(options);
        auto m = dt::load_metafile(output);
        CHECK(m.created_by() == options.created_by);
    }
}

TEST_CASE("test create app: comment")
{
    std::stringstream buffer {};
    temporary_directory tmp_dir {};

    fs::path output = fs::path(tmp_dir) / "test-create-comment.torrent";
    create_app_options create_options {
            .target = fs::path(TEST_DIR) / "resources",
            .destination = output,
    };

    SECTION("no comment") {
        auto options = create_options;
        options.comment = std::nullopt;
        run_create_app(options);
        auto m = dt::load_metafile(output);
        CHECK(m.comment().empty());
    }
    SECTION("comment") {
        auto options = create_options;
        options.comment = "comment";
        run_create_app(options);
        auto m = dt::load_metafile(output);
        CHECK(m.comment() == options.comment);
    }
}


TEST_CASE("test create app: io-block-size")
{
    using namespace dottorrent::literals;
    temporary_directory tmp_dir {};

    fs::path output = fs::path(tmp_dir) / "test-io-block-size.torrent";
    create_app_options options {
            .target = fs::path(TEST_DIR) / "resources",
            .destination = output,
    };

    SECTION("io-block-size smaller then piece-size") {
        options.piece_size = 2_MiB;
        options.io_block_size = 1_MiB;
        CHECK_THROWS(run_create_app(options));
    }
}