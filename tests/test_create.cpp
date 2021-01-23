
#include <experimental/source_location>

#include <catch2/catch.hpp>
#include <fmt/format.h>
#include <CLI/CLI.hpp>

#include <dottorrent/dht_node.hpp>
#include "create.hpp"
#include "tracker_database.hpp"

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
            CHECK(create_options.is_private == true);
        }
        SECTION("explicit off") {
            auto cmd = fmt::format("create {} --private off", file);
            PARSE_ARGS(cmd);
            CHECK(create_options.is_private == true);
        }
        SECTION("implicit on") {
            auto cmd = fmt::format("create {} --private", file);
            PARSE_ARGS(cmd);
            CHECK(create_options.is_private == true);
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
            auto cmd = fmt::format("create {} --include \"{}\"", file, "*.git.*");
            PARSE_ARGS(cmd);
            CHECK(create_options.include_patterns.size() == 1);
            CHECK(create_options.include_patterns.at(0) == "*.git.*");
        }
        SECTION("multiple values given") {
            auto cmd = fmt::format(R"(create {} --include "{}" "{}" --include-hidden)", file, "*.\\.git.*", ".*test.*" );
            PARSE_ARGS(cmd);
            CHECK(create_options.checksums.size() == 2);
            CHECK(create_options.include_patterns.at(0) == "*.git.*");
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
            CHECK(create_options.checksums.size() == 2);
            CHECK(create_options.exclude_patterns.at(0) == ".*\\.git.*");
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

}