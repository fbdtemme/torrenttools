
#include <experimental/source_location>

#include <catch2/catch_all.hpp>
#include <fmt/format.h>
#include <CLI/CLI.hpp>

#include <dottorrent/dht_node.hpp>
#include "create.hpp"
#include "tracker_database.hpp"
#include "test_resources.hpp"
#include "config_parser.hpp"

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
    auto create_app = app.add_subcommand("create", "Create a new metafile");
    create_app_options create_options{};
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

            CHECK(create_options.announce_list.size()==1);
            CHECK(create_options.announce_list.at(0)==std::vector{{tracker1}});
        }
        SECTION("multiple announces") {
            auto cmd = fmt::format("create {} --announce {} {} {}", file, tracker1, tracker2, tracker3);

            PARSE_ARGS(cmd);

            CHECK(create_options.announce_list.size()==3);
            CHECK(create_options.announce_list.at(0)==std::vector{{tracker1}});
            CHECK(create_options.announce_list.at(1)==std::vector{{tracker2}});
            CHECK(create_options.announce_list.at(2)==std::vector{{tracker3}});
        }
        SECTION("multiple tiers") {
            auto cmd = fmt::format("create {} --announce {} [{} {}]", file, tracker1, tracker2, tracker3);

            PARSE_ARGS(cmd);

            CHECK(create_options.announce_list.size()==2);
            CHECK(create_options.announce_list.at(0)==std::vector{{tracker1}});
            CHECK(create_options.announce_list.at(1)==std::vector{{tracker2, tracker3}});
        }
    }

    SECTION("announce group") {
        std::string group1 = "example-group1";
        std::string group2 = "example-group2";

        SECTION("single group") {
            auto cmd = fmt::format("create {} --announce-group {}", file, group2);
            PARSE_ARGS(cmd);
            CHECK(create_options.announce_group_list.size()==1);
        }

        SECTION("multiple groups") {
            auto cmd = fmt::format("create {} --announce-group {} {}", file, group1, group2);
            PARSE_ARGS(cmd);
            CHECK(create_options.announce_group_list.size()==2);
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

            CHECK(create_options.dht_nodes.size()==1);
            CHECK(create_options.dht_nodes.at(0)==dt::dht_node{"192.169.0.100", 6464});
        }

        SECTION("multiple dht node") {
            auto cmd = fmt::format("create {} --dht-node {} {}", file, node1, node2);
            PARSE_ARGS(cmd);

            CHECK(create_options.dht_nodes.size()==2);
            CHECK(create_options.dht_nodes.at(0)==dt::dht_node{"192.169.0.100", 6464});
            CHECK(create_options.dht_nodes.at(1)==dt::dht_node{"8.8.8.8", 80});
        }

        SECTION("bad dht node") {
            auto cmd = fmt::format("create {} --dht-node {}", file, invalid_node);
            CHECK_THROWS(PARSE_ARGS_THROWING(cmd));

            CHECK(create_options.dht_nodes.size()==0);
        }
    }

    SECTION("test web seeds")
    {
        std::string seed1 = "https://web-seed-url.com/files1";
        std::string seed2 = "ftp://web-seed-url.com/files2";


        SECTION("single web-seed ") {
            auto cmd = fmt::format("create {} --web-seed {}", file, seed1);
            PARSE_ARGS(cmd);

            CHECK(create_options.web_seeds.size()==1);
            CHECK(create_options.web_seeds.at(0)==seed1);
        }

        SECTION("multiple web seeds") {
            auto cmd = fmt::format("create {} --web-seed {} {}", file, seed1, seed2);
            PARSE_ARGS(cmd);

            CHECK(create_options.web_seeds.size()==2);
            CHECK(create_options.web_seeds.at(0)==seed1);
            CHECK(create_options.web_seeds.at(1)==seed2);
        }
    }
    SECTION("test http seeds")
    {
        std::string seed1 = "https://http-seed-url.com/files1";
        std::string seed2 = "https://htp-seed-url.com/files2";

        SECTION("single web-seed ") {
            auto cmd = fmt::format("create {} --http-seed {}", file, seed1);
            PARSE_ARGS(cmd);

            CHECK(create_options.http_seeds.size()==1);
            CHECK(create_options.http_seeds.at(0)==seed1);
        }

        SECTION("multiple web seeds") {
            auto cmd = fmt::format("create {} --http-seed {} {}", file, seed1, seed2);
            PARSE_ARGS(cmd);

            CHECK(create_options.http_seeds.size()==2);
            CHECK(create_options.http_seeds.at(0)==seed1);
            CHECK(create_options.http_seeds.at(1)==seed2);
        }
    }
    SECTION("test comment")
    {
        SECTION("set comment") {
            std::string comment = "Test comment";
            auto cmd = fmt::format("create {} --comment \"{}\"", file, comment);
            PARSE_ARGS(cmd);

            CHECK(create_options.comment==comment);
        }
        SECTION("no comment given") {
            auto cmd = fmt::format("create {}", file);
            PARSE_ARGS(cmd);
            CHECK(create_options.comment==std::nullopt);
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
            CHECK(*create_options.piece_size==1 << 20);
        }

        SECTION("in KiB") {
            auto cmd = fmt::format("create {} --piece-size {}", file, "1024K");
            PARSE_ARGS(cmd);
            CHECK(create_options.piece_size.has_value());
            CHECK(*create_options.piece_size==1 << 20);
        }

        SECTION("in MiB") {
            auto cmd = fmt::format("create {} --piece-size \"{}\"", file, "1M");
            PARSE_ARGS(cmd);
            CHECK(create_options.piece_size.has_value());
            CHECK(*create_options.piece_size==1 << 20);
        }

        SECTION("space between unit and value") {
            auto cmd = fmt::format("create {} --piece-size \"{}\"", file, "1 MiB");
            PARSE_ARGS(cmd);
            CHECK(create_options.piece_size.has_value());
            CHECK(*create_options.piece_size==1 << 20);
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
            CHECK(create_options.threads==2);
        }
        SECTION("option given") {
            auto cmd = fmt::format("create {} --threads 4", file);
            PARSE_ARGS(cmd);
            CHECK(create_options.threads==4);
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
            CHECK(*create_options.source=="My Tracker");
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
            CHECK(create_options.name=="renamed-file");
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
            CHECK(create_options.checksums.size()==1);
            CHECK(create_options.checksums.find(dt::hash_function::sha512)!=create_options.checksums.end());
        }
        SECTION("multiple checksum given") {
            auto cmd = fmt::format(R"(create {} --checksum "{}" "{}")", file, "sha512", "blake2b_512");
            PARSE_ARGS(cmd);
            CHECK(create_options.checksums.size()==2);
            CHECK(create_options.checksums.find(dt::hash_function::sha512)!=create_options.checksums.end());
            CHECK(create_options.checksums.find(dt::hash_function::blake2b_512)!=create_options.checksums.end());
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
            CHECK(create_options.creation_date
                    ==std::chrono::system_clock::time_point(std::chrono::seconds(1611339706)));
        }
        SECTION("POSIX time - no seperators") {
            auto cmd = fmt::format("create {} --creation-date {}", file, "2021-01-22T18:21:46Z+0100");
            PARSE_ARGS(cmd);
            CHECK(create_options.creation_date.has_value());
            CHECK(create_options.creation_date
                    ==std::chrono::system_clock::time_point(std::chrono::seconds(1611339706)));
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
            CHECK(create_options.include_patterns.size()==1);
            CHECK(create_options.include_patterns.at(0)==".*\\.git.*");
        }
        SECTION("multiple values given") {
            auto cmd = fmt::format(R"(create {} --include "{}" "{}" --include-hidden)", file, "*.\\.git.*", ".*test.*");
            PARSE_ARGS(cmd);
            CHECK(create_options.include_patterns.size()==2);
            CHECK(create_options.include_patterns.at(0)=="*.\\.git.*");
            CHECK(create_options.include_patterns.at(1)==".*test.*");
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
            CHECK(create_options.exclude_patterns.size()==1);
            CHECK(create_options.exclude_patterns.at(0)=="*.git.*");
        }
        SECTION("multiple values given") {
            auto cmd = fmt::format(R"(create {} --exclude "{}" "{}" --include-hidden)", file, "*.\\.git.*", ".*test.*");
            PARSE_ARGS(cmd);
            CHECK(create_options.exclude_patterns.size()==2);
            CHECK(create_options.exclude_patterns.at(0)=="*.\\.git.*");
            CHECK(create_options.exclude_patterns.at(1)==".*test.*");
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
            CHECK(*create_options.io_block_size==1 << 20);
        }

        SECTION("in KiB") {
            auto cmd = fmt::format("create {} --io-block-size {}", file, "1024K");
            PARSE_ARGS(cmd);
            CHECK(create_options.io_block_size.has_value());
            CHECK(*create_options.io_block_size==1 << 20);
        }

        SECTION("in MiB") {
            auto cmd = fmt::format("create {} --io-block-size \"{}\"", file, "1M");
            PARSE_ARGS(cmd);
            CHECK(create_options.io_block_size.has_value());
            CHECK(*create_options.io_block_size==1 << 20);
        }
        SECTION("as power of two") {
            auto cmd = fmt::format("create {} --io-block-size {}", file, 20);
            PARSE_ARGS(cmd);
            CHECK(create_options.io_block_size.has_value());
            CHECK(*create_options.io_block_size==1 << 20);
        }

        SECTION("in KiB") {
            auto cmd = fmt::format("create {} --io-block-size {}", file, "1024K");
            PARSE_ARGS(cmd);
            CHECK(create_options.io_block_size.has_value());
            CHECK(*create_options.io_block_size==1 << 20);
        }

        SECTION("in MiB") {
            auto cmd = fmt::format("create {} --io-block-size \"{}\"", file, "1M");
            PARSE_ARGS(cmd);
            CHECK(create_options.io_block_size.has_value());
            CHECK(*create_options.io_block_size==1 << 20);
        }

        SECTION("space between unit and value") {
            auto cmd = fmt::format("create {} --io-block-size \"{}\"", file, "1 MiB");
            PARSE_ARGS(cmd);
            CHECK(create_options.io_block_size.has_value());
            CHECK(*create_options.io_block_size==1 << 20);
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
            CHECK(*create_options.io_block_size==1 << 20);
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
            auto output = fs::path("/home/test/output.torrent");
            auto cmd = fmt::format("create {} --output {}", file, output.string());
            PARSE_ARGS(cmd);
#if defined(__APPLE__)
            auto new_fs_equal =
                    create_options.destination->lexically_relative("/System/Volumes/Data")==output.relative_path();
            auto old_fs_equal = *create_options.destination==output;
            CHECK((new_fs_equal || old_fs_equal));
#else

            CHECK(create_options.destination->string() ==  fs::absolute(output).string());
#endif
        }
        SECTION("Directory with trailing slash") {
            auto output = fs::path("/home/test/Downloads/");
            auto cmd = fmt::format("create {} --output {}", file, output.string());
            PARSE_ARGS(cmd);
#if defined(__APPLE__)
            auto new_fs_equal =
                    create_options.destination->lexically_relative("/System/Volumes/Data")==output.relative_path();
            auto old_fs_equal = *create_options.destination==output;
            CHECK((new_fs_equal || old_fs_equal));
#else
            CHECK(create_options.destination->string() == fs::absolute(output).string());
#endif
        }
        SECTION("Existing directory -> append trailing slash") {
            temporary_directory tmp_dir{};
            auto output = tmp_dir.path();
            auto expected = output / "";
            auto cmd = fmt::format("create {} --output {}", file, output.string());

            PARSE_ARGS(cmd);
#if defined(__APPLE__)
            auto new_fs_equal =
                    create_options.destination->lexically_relative("/private") == expected.relative_path();
            auto old_fs_equal = *create_options.destination==expected;
            CHECK((new_fs_equal || old_fs_equal));
#else
            CHECK(*create_options.destination == expected);
#endif
        }
    }

    SECTION("protocol") {
        SECTION("v1") {
            auto cmd = fmt::format("create {} --protocol {}", file, "v1");
            PARSE_ARGS(cmd);
            CHECK(create_options.protocol_version==dt::protocol::v1);
        }
        SECTION("v2") {
            auto cmd = fmt::format("create {} --protocol {}", file, "v2");
            PARSE_ARGS(cmd);
            CHECK(create_options.protocol_version==dt::protocol::v2);
        }
        SECTION("hybrid") {
            auto cmd = fmt::format("create {} --protocol {}", file, "hybrid");
            PARSE_ARGS(cmd);
            CHECK(create_options.protocol_version==dt::protocol::hybrid);
        }
        SECTION("1") {
            auto cmd = fmt::format("create {} --protocol {}", file, "1");
            PARSE_ARGS(cmd);
            CHECK(create_options.protocol_version==dt::protocol::v1);
        }
        SECTION("2") {
            auto cmd = fmt::format("create {} --protocol {}", file, "2");
            PARSE_ARGS(cmd);
            CHECK(create_options.protocol_version==dt::protocol::v2);
        }
    }
    SECTION("similar") {
        SECTION("single infohash") {
            auto sha1_hex = "03a23323650e652f4e89b4febf66b39a546781b8";
            auto cmd = fmt::format("create {} --similar {}", file, sha1_hex);
            PARSE_ARGS(cmd);
            CHECK(create_options.similar_torrents.size()==1);
            CHECK(create_options.similar_torrents[0]
                    ==dt::make_hash_from_hex<dt::sha1_hash>(sha1_hex));
        }
        SECTION("multiple infohashes") {
            auto sha1_hex1 = "03a23323650e652f4e89b4febf66b39a546781b8";
            auto sha1_hex2 = "5829e7a3b4810e5ee9283137e7ad62d0e88dfd3f";

            auto cmd = fmt::format("create {} --similar {} {}", file, sha1_hex1, sha1_hex2);
            PARSE_ARGS(cmd);
            CHECK(create_options.similar_torrents.size()==2);
            CHECK(create_options.similar_torrents[0]
                    ==dt::make_hash_from_hex<dt::sha1_hash>(sha1_hex1));
            CHECK(create_options.similar_torrents[1]
                    ==dt::make_hash_from_hex<dt::sha1_hash>(sha1_hex2));
        }
        SECTION("single torrent") {
            auto tf = fs::path(TEST_DIR)/"resources"/"Fedora-Workstation-Live-x86_64-30.torrent";
            auto m = dt::load_metafile(tf);
            auto cmd = fmt::format("create {} --similar {}", file, tf.string());
            PARSE_ARGS(cmd);
            CHECK(create_options.similar_torrents.size()==1);
            CHECK(create_options.similar_torrents[0]==dt::info_hash_v1(m));
        }
        SECTION("multiple torrents") {
            auto tf1 = fs::path(TEST_DIR)/"resources"/"Fedora-Workstation-Live-x86_64-30.torrent";
            auto tf2 = fs::path(TEST_DIR)/"resources"/"CAMELYON17.torrent";

            auto m1 = dt::load_metafile(tf1);
            auto m2 = dt::load_metafile(tf2);

            auto cmd = fmt::format("create {} --similar {} {}", file, tf1.string(), tf2.string());
            PARSE_ARGS(cmd);
            CHECK(create_options.similar_torrents.size()==2);
            CHECK(create_options.similar_torrents[0]==dt::info_hash_v1(m1));
            CHECK(create_options.similar_torrents[1]==dt::info_hash_v1(m2));
        }

        SECTION("invalid infohash length") {
            auto sha1_hex = "03a23323650e652fab9120db0214e89b4febf66b39a546781b8";
            auto cmd = fmt::format("create {} --similar {}", file, sha1_hex);
            CHECK_THROWS(PARSE_ARGS_THROWING(cmd));
        }

        SECTION("invalid hexadecimal string") {
            auto sha1_hex = "03a2332djovpwomdq";
            auto cmd = fmt::format("create {} --similar {}", file, sha1_hex);
            CHECK_THROWS(PARSE_ARGS_THROWING(cmd));
        }

        SECTION("mixed infohash and file") {
            auto sha1_hex1 = "03a23323650e652f4e89b4febf66b39a546781b8";
            auto tf1 = fs::path(TEST_DIR)/"resources"/"Fedora-Workstation-Live-x86_64-30.torrent";
            auto m1 = dt::load_metafile(tf1);

            auto cmd = fmt::format("create {} --similar {} {}", file, sha1_hex1, tf1.string());
            PARSE_ARGS(cmd);
            CHECK(create_options.similar_torrents.size()==2);
            CHECK(create_options.similar_torrents[0]==dt::make_hash_from_hex<dt::sha1_hash>(sha1_hex1));
            CHECK(create_options.similar_torrents[1]==dt::info_hash_v1(m1));
        }

        SECTION("hybrid torrent") {
            auto tf1 = fs::path(TEST_DIR)/"resources"/"bittorrent-v2-hybrid-test.torrent";
            auto m1 = dt::load_metafile(tf1);
            auto cmd = fmt::format("create {} --similar {}", file, tf1.string());
            auto v1_hash = dt::info_hash::from_hex(dt::protocol::v1, "631a31dd0a46257d5078c0dee4e66e26f73e42ac");
            auto v2_hash = dt::info_hash::from_hex(dt::protocol::v2,
                    "d8dd32ac93357c368556af3ac1d95c9d76bd0dff6fa9833ecdac3d53134efabb");

            PARSE_ARGS(cmd);
            CHECK(create_options.similar_torrents.size()==2);
            CHECK(create_options.similar_torrents.at(0)==v1_hash);
            CHECK(create_options.similar_torrents.at(1)==v2_hash);
        }

        SECTION("v2 torrent") {
            auto tf1 = fs::path(TEST_DIR)/"resources"/"bittorrent-v2-test.torrent";
            auto m1 = dt::load_metafile(tf1);
            auto cmd = fmt::format("create {} --similar {}", file, tf1.string());
            auto v2_hash = dt::info_hash::from_hex(dt::protocol::v2,
                    "caf1e1c30e81cb361b9ee167c4aa64228a7fa4fa9f6105232b28ad099f3a302e");

            PARSE_ARGS(cmd);
            CHECK(create_options.similar_torrents.size()==1);
            CHECK(create_options.similar_torrents.at(0)==v2_hash);
        }
    }

    SECTION("collections") {
        SECTION("single collection") {
            auto cmd = fmt::format("create {} --collection {}", file, "test1");
            PARSE_ARGS(cmd);
            CHECK(create_options.collections==std::vector<std::string>{"test1"});
        }
        SECTION("multiple collections") {
            auto cmd = fmt::format("create {} --collection {} {}", file, "test1", "test2");
            PARSE_ARGS(cmd);
            CHECK(create_options.collections==std::vector<std::string>{"test1", "test2"});
        }
    }
    SECTION("no-cross-seed") {
        SECTION("no flag given collection") {
            auto cmd = fmt::format("create {}", file);
            PARSE_ARGS(cmd);
            CHECK(create_options.enable_cross_seeding);
        }
        SECTION("no flag given collection") {
            auto cmd = fmt::format("create {} --no-cross-seed", file);
            PARSE_ARGS(cmd);
            CHECK_FALSE(create_options.enable_cross_seeding);
        }
    }
}

TEST_CASE("test create app: target")
{
    temporary_directory tmp_dir{};
    main_app_options main_options{};

    SECTION("target is file") {
        fs::path target = camelyon_torrent;
        fs::path output = fs::path(tmp_dir)/"test-file-target.torrent";

        create_app_options options{
                .target = target,
                .destination = output,
                .protocol_version = dt::protocol::v1
        };
        run_create_app(main_options, options);
        auto m = dt::load_metafile(output);
        const auto& storage = m.storage();
        CHECK(storage.size()==1);
        CHECK(storage.at(0).path().filename()==camelyon_torrent.filename());
    }
    SECTION("target is directory") {
        fs::path target = TEST_RESOURCES_DIR;
        fs::path output = fs::path(tmp_dir)/"test-directory-target.torrent";

        create_app_options options{.target = target, .destination = output,};
        run_create_app(main_options, options);
        auto m = dt::load_metafile(output);
        const auto& storage = m.storage();
        CHECK(m.name()==target.filename());
    }
    SECTION("target is directory with single file")
    {
        auto root_path = fs::path(tmp_dir) / "dir_with_single_file_torrent";
        fs::create_directories(root_path);
        std::ofstream ofs(root_path / "test");
        ofs << "test_file\n";

        fs::path target = root_path;
        fs::path output = fs::path(tmp_dir)/ "test-dir-with-single-file-torrent.torrent";

        create_app_options options{.target = target, .destination = output,};
        run_create_app(main_options, options);
        auto m = dt::load_metafile(output);
        const auto& storage = m.storage();
        CHECK(m.name() == "dir_with_single_file_torrent");
        CHECK(m.storage().file_mode() == dt::file_mode::multi);
    }
}

TEST_CASE("test create app: announce-url")
{
    temporary_directory tmp_dir{};
    const auto* db = torrenttools::load_tracker_database();
    const auto* config = torrenttools::load_config();
    main_app_options main_options{};

    fs::path target = fs::path(TEST_RESOURCES_DIR);

    create_app_options options{
            .target = target,
            .destination = tmp_dir.path(),
            .protocol_version = dt::protocol::v1,
    };

    SECTION("tracker abbreviation") {
        fs::path expected_destination = tmp_dir.path()/fmt::format("[HDB]{}.torrent", target.filename().string());

        const auto& db_entry = db->at("hdb");
        options.announce_list = {{"hdb"}};
        run_create_app(main_options, options);

        REQUIRE(fs::exists(expected_destination));
        auto m = dt::load_metafile(expected_destination);

        CHECK(m.is_private()==db_entry.is_private);
        CHECK(m.source()==db_entry.name);
        CHECK(m.trackers().at(0).url==db_entry.substitute_parameters(*config));
    }
}

TEST_CASE("test create app: announce-group")
{
    temporary_directory tmp_dir{};
    const auto* db = torrenttools::load_tracker_database();
    const auto* config = torrenttools::load_config();
    main_app_options main_options{};

    fs::path target = fs::path(TEST_RESOURCES_DIR);

    create_app_options options{
            .target = target,
            .destination = tmp_dir.path(),
            .protocol_version = dt::protocol::v1,
    };

    SECTION("announce urls") {
        fs::path expected_destination = tmp_dir.path()/fmt::format("{}.torrent", target.filename().string());

        options.protocol_version = dt::protocol::v1;
        options.announce_group_list = {{"public-trackers"}};
        run_create_app(main_options, options);

        REQUIRE(fs::exists(expected_destination));
        auto m = dt::load_metafile(expected_destination);

        CHECK(m.is_private()==false);

        CHECK(m.trackers().at(0).url=="http://tracker.opentrackr.org:1337/announce");
        CHECK(m.trackers().at(1).url=="udp://tracker.openbittorrent.com:6969/announce");
    }

    SECTION("named trackers") {
        fs::path expected_destination = tmp_dir.path()/fmt::format("{}.torrent", target.filename().string());

        options.announce_group_list = {{"example-group"}};
        run_create_app(main_options, options);

        REQUIRE(fs::exists(expected_destination));
        auto m = dt::load_metafile(expected_destination);

        auto ar_url = db->at("AlphaRatio").substitute_parameters(*config);
        auto tl_url = db->at("TorrentLeech").substitute_parameters(*config);

        CHECK(m.trackers().at(0).url==ar_url);
        CHECK(m.trackers().at(1).url==tl_url);
        CHECK(m.is_private());
    }
}

TEST_CASE("test create app: source tag")
{
    temporary_directory tmp_dir{};
    main_app_options main_options{};

    fs::path output = fs::path(tmp_dir)/"test-create-source.torrent";

    create_app_options create_options{
            .target = fs::path(TEST_DIR)/"resources",
            .destination = output,
            .protocol_version = dt::protocol::v1,
            .source = "test",
    };
    run_create_app(main_options, create_options);
    auto m = dt::load_metafile(output);

    CHECK(m.source()==create_options.source);
}

TEST_CASE("test create app: web-seeds")
{
    temporary_directory tmp_dir{};
    main_app_options main_options{};

    fs::path output = fs::path(tmp_dir)/"test-create-webseed.torrent";

    create_app_options create_options{
            .target = fs::path(TEST_DIR)/"resources",
            .destination = output,
            .protocol_version = dt::protocol::v1,
            .web_seeds = {"https://test1.com", "https://test2.com"},
    };
    run_create_app(main_options, create_options);
    auto m = dt::load_metafile(output);

    CHECK(m.web_seeds().size()==2);
    CHECK(m.web_seeds().at(0)=="https://test1.com");
    CHECK(m.web_seeds().at(1)=="https://test2.com");
}

TEST_CASE("test create app: http-seeds")
{
    temporary_directory tmp_dir{};
    main_app_options main_options{};

    fs::path output = fs::path(tmp_dir)/"test-create-http-seeds.torrent";

    create_app_options create_options{
            .target = fs::path(TEST_DIR)/"resources",
            .destination = output,
            .protocol_version = dt::protocol::v1,
            .http_seeds = {"https://test1.com", "https://test2.com"},
    };
    run_create_app(main_options, create_options);
    auto m = dt::load_metafile(output);

    CHECK(m.http_seeds().size()==2);
    CHECK(m.http_seeds().at(0)=="https://test1.com");
    CHECK(m.http_seeds().at(1)=="https://test2.com");
}

TEST_CASE("test create app: dht-nodes")
{
    temporary_directory tmp_dir{};
    main_app_options main_options{};

    fs::path output = fs::path(tmp_dir)/"test-create-webseed.torrent";

    create_app_options create_options{
            .target = fs::path(TEST_DIR)/"resources",
            .destination = output,
            .protocol_version = dt::protocol::v1,
            .dht_nodes = {{"127.0.0.1",              6969},
                          {"https://my-dht-node.de", 8888}},
    };
    run_create_app(main_options, create_options);
    auto m = dt::load_metafile(output);

    CHECK(m.dht_nodes().size()==2);
    CHECK(m.dht_nodes().at(0)==dt::dht_node{"127.0.0.1", 6969});
    CHECK(m.dht_nodes().at(1)==dt::dht_node{"https://my-dht-node.de", 8888});
}

TEST_CASE("test create app: private flag")
{
    temporary_directory tmp_dir{};
    main_app_options main_options{};

    fs::path output = fs::path(tmp_dir)/"test-create-private.torrent";

    create_app_options create_options{
            .target = fs::path(TEST_DIR)/"resources",
            .destination = output,
            .protocol_version = dt::protocol::v1,
    };

    SECTION("true") {
        auto options = create_options;
        options.is_private = true;
        run_create_app(main_options, options);
        auto m = dt::load_metafile(output);
        CHECK(m.is_private()==options.is_private);
    }
    SECTION("false") {
        auto options = create_options;
        options.is_private = true;
        run_create_app(main_options, options);
        auto m = dt::load_metafile(output);
        CHECK(m.is_private()==options.is_private);
    }
}

TEST_CASE("test create app: creation-date")
{
    std::stringstream buffer{};
    temporary_directory tmp_dir{};
    main_app_options main_options{};

    fs::path output = fs::path(tmp_dir)/"test-edit-creation-date.torrent";
    create_app_options create_options{
            .target = fs::path(TEST_DIR)/"resources",
            .destination = output,
            .protocol_version = dt::protocol::v1,
            .set_creation_date = true,
    };

    SECTION("--no-creation-date") {
        auto options = create_options;
        options.set_creation_date = false;
        run_create_app(main_options, options);
        auto m = dt::load_metafile(output);
        CHECK(m.creation_date()==std::chrono::seconds(0));
    }
    SECTION("overriding creation date") {
        auto options = create_options;
        options.creation_date = std::chrono::system_clock::time_point(std::chrono::seconds(1611339706));
        run_create_app(main_options, options);
        auto m = dt::load_metafile(output);
        CHECK(m.creation_date()==std::chrono::seconds(1611339706));
    }
}

TEST_CASE("test create app: created-by")
{
    std::stringstream buffer{};
    temporary_directory tmp_dir{};
    main_app_options main_options{};

    fs::path output = fs::path(tmp_dir)/"test-create-created-by.torrent";
    create_app_options create_options{
            .target = fs::path(TEST_DIR)/"resources",
            .destination = output,
            .protocol_version = dt::protocol::v1,
    };

    SECTION("--no-created-by") {
        auto options = create_options;
        options.set_created_by = false;
        run_create_app(main_options, options);
        auto m = dt::load_metafile(output);
        CHECK(m.created_by().empty());
    }
    SECTION("overriding created-by") {
        auto options = create_options;
        options.created_by = "me";
        run_create_app(main_options, options);
        auto m = dt::load_metafile(output);
        CHECK(m.created_by()==options.created_by);
    }
}

TEST_CASE("test create app: comment")
{
    std::stringstream buffer{};
    temporary_directory tmp_dir{};
    main_app_options main_options{};

    fs::path output = fs::path(tmp_dir)/"test-create-comment.torrent";
    create_app_options create_options{
            .target = fs::path(TEST_DIR)/"resources",
            .destination = output,
            .protocol_version = dt::protocol::v1,
    };

    SECTION("no comment") {
        auto options = create_options;
        options.comment = std::nullopt;
        run_create_app(main_options, options);
        auto m = dt::load_metafile(output);
        CHECK(m.comment().empty());
    }
    SECTION("comment") {
        auto options = create_options;
        options.comment = "comment";
        run_create_app(main_options, options);
        auto m = dt::load_metafile(output);
        CHECK(m.comment()==options.comment);
    }
}

TEST_CASE("test create app: io-block-size")
{
    using namespace dottorrent::literals;
    temporary_directory tmp_dir{};
    main_app_options main_options{};

    fs::path output = fs::path(tmp_dir)/"test-io-block-size.torrent";
    create_app_options options{
            .target = fs::path(TEST_DIR)/"resources",
            .destination = output,
            .protocol_version = dt::protocol::v1,
    };

    SECTION("io-block-size smaller then piece-size") {
        options.piece_size = 2_MiB;
        options.io_block_size = 1_MiB;
        CHECK_THROWS(run_create_app(main_options, options));
    }
}

TEST_CASE("test create app: protocol")
{
    using namespace dottorrent::literals;
    temporary_directory tmp_dir{};
    main_app_options main_options{};

    create_app_options options{
            .target = fs::path(TEST_DIR)/"resources",
    };

    SECTION("v1") {
        options.destination = fs::path(tmp_dir)/"test-protocol-v1.torrent";
        options.protocol_version = dt::protocol::v1;
        run_create_app(main_options, options);
        auto m = dt::load_metafile(*options.destination);
        CHECK(m.storage().protocol()==dt::protocol::v1);
    }

    SECTION("v2") {
        options.destination = fs::path(tmp_dir)/"test-protocol-v2.torrent";
        options.protocol_version = dt::protocol::v2;
        run_create_app(main_options, options);
        auto m = dt::load_metafile(*options.destination);
        CHECK(m.storage().protocol()==dt::protocol::v2);
    }

    SECTION("hybrid") {
        options.destination = fs::path(tmp_dir)/"test-protocol-hybrid.torrent";
        options.protocol_version = dt::protocol::hybrid;
        run_create_app(main_options, options);
        auto m = dt::load_metafile(*options.destination);
        CHECK(m.storage().protocol()==dt::protocol::hybrid);
    }
}

TEST_CASE("test create app: similar torrents")
{
    using namespace dottorrent::literals;
    temporary_directory tmp_dir{};
    main_app_options main_options{};

    fs::path output = fs::path(tmp_dir)/"test-create-similar-torrents.torrent";

    create_app_options options{
            .target = fs::path(TEST_DIR)/"resources",
            .destination = output,
            .protocol_version = dt::protocol::v1,
    };

    auto v1_hash = dt::info_hash::from_hex(dt::protocol::v1, "631a31dd0a46257d5078c0dee4e66e26f73e42ac");
    auto v2_hash = dt::info_hash::from_hex(dt::protocol::v2,
            "d8dd32ac93357c368556af3ac1d95c9d76bd0dff6fa9833ecdac3d53134efabb");

    SECTION("v1") {
        options.destination = fs::path(tmp_dir)/"test-similar-v1.torrent";
        options.protocol_version = dt::protocol::v1;
        options.similar_torrents.emplace_back(v1_hash);

        run_create_app(main_options, options);
        auto m = dt::load_metafile(*options.destination);
        CHECK(m.similar_torrents().contains(v1_hash));
    }

    SECTION("v2") {
        options.destination = fs::path(tmp_dir)/"test-similar-v2.torrent";
        options.protocol_version = dt::protocol::v2;
        options.similar_torrents.emplace_back(v2_hash);

        run_create_app(main_options, options);
        auto m = dt::load_metafile(*options.destination);
        CHECK(m.similar_torrents().contains(v2_hash));
    }

    SECTION("hybrid") {
        options.destination = fs::path(tmp_dir)/"test-protocol-hybrid.torrent";
        options.protocol_version = dt::protocol::hybrid;
        options.similar_torrents.emplace_back(v1_hash);
        options.similar_torrents.emplace_back(v2_hash);

        run_create_app(main_options, options);
        auto m = dt::load_metafile(*options.destination);
        CHECK(m.similar_torrents().contains(v1_hash));
        CHECK(m.similar_torrents().contains(v2_hash));
    }
}

TEST_CASE("test create app: collection")
{
    using namespace dottorrent::literals;
    temporary_directory tmp_dir{};
    main_app_options main_options{};

    create_app_options options{
            .target = fs::path(TEST_DIR)/"resources",
            .destination = fs::path(tmp_dir)/"test-create-collection.torrent",
            .protocol_version = dt::protocol::v1,
    };

    SECTION("single collection") {
        options.collections = {"test1"};
        run_create_app(main_options, options);
        auto m = dt::load_metafile(*options.destination);
        CHECK(m.collections().contains("test1"));
    }

    SECTION("multiple collections") {
        options.collections = {"test1", "test2"};
        run_create_app(main_options, options);
        auto m = dt::load_metafile(*options.destination);
        CHECK(m.collections().contains("test1"));
        CHECK(m.collections().contains("test2"));
    }
}


TEST_CASE("test create app: cross-seeding")
{
    temporary_directory tmp_dir{};
    main_app_options main_options{};

    SECTION("include cross-seed entry") {
        fs::path output = fs::path(tmp_dir)/"test-cross-seeding-tags.torrent";

        create_app_options create_options{
            .target = fs::path(TEST_DIR)/"resources",
            .destination = output,
            .protocol_version = dt::protocol::v1,
            .enable_cross_seeding = true
        };
        run_create_app(main_options, create_options);
        auto m = dt::load_metafile(output);
        CHECK(m.other_info_fields().contains("cross_seed_entry"));
    }
    SECTION("exclude cross-seed entry") {
        fs::path output = fs::path(tmp_dir)/"test-cross-seeding-tags.torrent";

        create_app_options create_options{
            .target = fs::path(TEST_DIR)/"resources",
            .destination = output,
            .protocol_version = dt::protocol::v1,
            .enable_cross_seeding = false
        };
        run_create_app(main_options, create_options);
        auto m = dt::load_metafile(output);
        CHECK_FALSE(m.other_info_fields().contains("cross_seed_entry"));
    }
}