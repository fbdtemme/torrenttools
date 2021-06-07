#include <filesystem>
#include <string>
#include <variant>

#include <catch2/catch.hpp>
#include <yaml-cpp/yaml.h>

#include "profile.hpp"
#include "config_parser.hpp"
#include "exceptions.hpp"

namespace fs = std::filesystem;
using namespace torrenttools;
using namespace std::string_literals;

#define GET_TEST_OPTIONS(NAME) \
auto cfg = config(NAME); \
const auto& options = std::get<create_app_options>(cfg.get_profile("test").options);

TEST_CASE("profile parsing")
{
    SECTION("bad profile value type") {
        std::string p = R"(
profiles: []
)";
        CHECK_THROWS_AS(config(p), profile_error);
    }

    SECTION("missing options key") {
        std::string p = R"(
profiles:
  test:
    command: create
)";
        CHECK_THROWS_AS(config(p), profile_error);
    }

    SECTION("missing command key") {
        std::string p = R"(
profiles:
  test:
    options: {}
)";
        CHECK_THROWS_AS(config(p), profile_error);
    }

    SECTION("duplicate profile name") {
        std::string p = R"(
profiles:
  test:
    command: create
    options: {}
  test:
    command: create
    options: {}
)";
        CHECK_THROWS_AS(config(p), profile_error);
    }
}

TEST_CASE("create profile parsing")
{
    SECTION("announce") {
        SECTION("without tiers") {
            std::string p = R"(
profiles:
  test:
    command: "create"
    options:
      announce: [ http://test-url1.com, http://test-url2.com ]
)";
            GET_TEST_OPTIONS(p);
            CHECK(options.announce_list==std::vector<std::vector<std::string>>(
                    {{"http://test-url1.com"},
                     {"http://test-url2.com"}}));
        }

        SECTION("with tiers") {
            std::string p = R"(
profiles:
  test:
    command: "create"
    options:
      announce:
        - [http://test-url1.com, http://test-url2.com]
        - http://test-url3.com
)";
            GET_TEST_OPTIONS(p);
            CHECK(options.announce_list==std::vector<std::vector<std::string>>(
                    {{"http://test-url1.com", "http://test-url2.com"},
                     {"http://test-url3.com"}}));
        }

        SECTION("invalid type - not a list") {
            std::string p = R"(
profiles:
  test:
    command: "create"
    options:
      announce: 3123
)";
            CHECK_THROWS_AS(config(p), profile_error);
        }

        SECTION("invalid type - invalid url type") {
            std::string p = R"(
profiles:
  test:
    command: "create"
    options:
      announce: [ {test: 1} ]
)";
            CHECK_THROWS_AS(config(p), profile_error);
        }
    }

    SECTION("announce group") {
        SECTION("valid") {
            std::string p = R"(
profiles:
  test:
    command: "create"
    options:
      announce-group: [ group1 ]
)";
            GET_TEST_OPTIONS(p);
            CHECK(options.announce_group_list==std::vector{"group1"s});
        }

        SECTION("not a list ") {
            std::string p = R"(
profiles:
  test:
    command: "create"
    options:
      announce-group: group1
)";
            CHECK_THROWS(config(p));
        }
    }

    SECTION("checksum") {
        SECTION("invalid type") {
            std::string p = R"(
profiles:
  test:
    command: "create"
    options:
      checksum: "sha1"
)";
            CHECK_THROWS(config(p));
        }
        SECTION("invalid value") {
            std::string p = R"(
profiles:
  test:
    command: "create"
    options:
      checksum:
        - sha1
        - md5
)";
            GET_TEST_OPTIONS(p);
            CHECK(options.checksums.contains(dt::hash_function::sha1));
            CHECK(options.checksums.contains(dt::hash_function::md5));
        }
    }

    SECTION("collection") {
        SECTION("valid") {
            std::string p = R"(
profiles:
  test:
    command: "create"
    options:
      collection: ["key1", "key2"]
)";
            GET_TEST_OPTIONS(p);
            CHECK(options.collections==std::vector<std::string>{"key1", "key2"});
        }
        SECTION("invalid type") {
            std::string p = R"(
profiles:
  test:
    command: "create"
    options:
      collection: "invalid-type"
)";
            CHECK_THROWS_AS(config(p), profile_error);
        }
    }

    SECTION("comment") {
        SECTION("valid") {
            std::string p = R"(
profiles:
  test:
    command: "create"
    options:
      comment: "test comment"
)";
            GET_TEST_OPTIONS(p);
            CHECK(options.comment=="test comment");
        }
        SECTION("invalid type") {
            std::string p = R"(
profiles:
  test:
    command: "create"
    options:
      comment: [ "test" ]
)";
            CHECK_THROWS_AS(config(p), profile_error);
        }
    }

    SECTION("created-by") {

        SECTION("valid") {
            std::string p = R"(
profiles:
  test:
    command: "create"
    options:
      created-by: "created by test"
)";
            GET_TEST_OPTIONS(p);
            CHECK(options.created_by == "created by test");
        }

        SECTION("bad type") {
            std::string p = R"(
profiles:
  test:
    command: "create"
    options:
      created-by: [ "test" ]
)";
            CHECK_THROWS_AS(config(p), profile_error);
        }
    }

    SECTION("creation-date") {

        SECTION("valid timestamp") {
            std::string p = R"(
profiles:
  test:
    command: "create"
    options:
      creation-date: 1611339706
)";
            GET_TEST_OPTIONS(p);
            CHECK(options.creation_date == std::chrono::system_clock::time_point(std::chrono::seconds(1611339706)));
        }

        SECTION("valid ISO timestamp") {
            std::string p = R"(
profiles:
  test:
    command: "create"
    options:
      creation-date: "2021-01-22T18:21:46Z+0100"
)";
            GET_TEST_OPTIONS(p);
            CHECK(options.creation_date == std::chrono::system_clock::time_point(std::chrono::seconds(1611339706)));
        }

        SECTION("bad type") {
            std::string p = R"(
profiles:
  test:
    command: "create"
    options:
      creation-date: ["test"]
)";
            CHECK_THROWS_AS(config(p), profile_error);
        }
    }

    SECTION("dht-node") {
        SECTION("bad type") {
            std::string p = R"(
profiles:
  test:
    command: "create"
    options:
      dht-node: "http://test.node.com:3232"
)";
            CHECK_THROWS_AS(config(p), profile_error);
        }
        SECTION("valid") {
            std::string p = R"(
profiles:
  test:
    command: "create"
    options:
      dht-node:
        - http://test1.node.com:3232
        - http://test2.node.com:6868
)";
            GET_TEST_OPTIONS(p);
            CHECK(options.dht_nodes == std::vector{dt::dht_node("http://test1.node.com", 3232),
                                                   dt::dht_node("http://test2.node.com", 6868)});
        }
    }

    SECTION("exclude") {
        SECTION("bad type") {
            std::string p = R"(
profiles:
  test:
    command: "create"
    options:
      exclude: "test"
)";
            CHECK_THROWS_AS(config(p), profile_error);
        }
    }

    SECTION("http-seed") {
        SECTION("valid") {
            std::string p = R"(
profiles:
  test:
    command: "create"
    options:
      http-seed:
        - https://seed1.com
        - https://seed2.com
)";
            GET_TEST_OPTIONS(p);
            CHECK(options.http_seeds == std::vector{"https://seed1.com"s, "https://seed2.com"s});
        }

        SECTION("bad type") {
            std::string p = R"(
profiles:
  test:
    command: "create"
    options:
      http-seed: "test"
)";
            CHECK_THROWS_AS(config(p), profile_error);
        }
    }

    SECTION("include") {
        SECTION("bad type") {
            std::string p = R"(
profiles:
  test:
    command: "create"
    options:
      include: "test"
)";
            CHECK_THROWS_AS(config(p), profile_error);
        }
    }
    SECTION("include-hidden") {
        SECTION("bad type") {
            std::string p = R"(
profiles:
  test:
    command: "create"
    options:
      include-hidden: "test"
)";
            CHECK_THROWS_AS(config(p), profile_error);
        }

        SECTION("valid") {
            std::string p = R"(
profiles:
  test:
    command: "create"
    options:
      include-hidden: false
)";
            GET_TEST_OPTIONS(p);
            CHECK(options.include_hidden_files == false);
        }
    }

    SECTION("io-block-size") {
        SECTION("valid") {
            std::string p = R"(
profiles:
  test:
    command: "create"
    options:
      io-block-size: 1M
)";
            GET_TEST_OPTIONS(p);
            CHECK(options.io_block_size == 1048576);
        }
        SECTION("bad type") {
            std::string p = R"(
profiles:
  test:
    command: "create"
    options:
      io-block-size: [ "test" ]
)";
            CHECK_THROWS_AS(config(p), profile_error);
        }
    }
    SECTION("name") {
        SECTION("valid") {
            std::string p = R"(
profiles:
  test:
    command: "create"
    options:
      name: "test"
)";
            GET_TEST_OPTIONS(p)
            CHECK(options.name == "test");
        }
        SECTION("bad type") {
            std::string p = R"(
profiles:
  test:
    command: "create"
    options:
      name: [ "test" ]
)";
            CHECK_THROWS_AS(config(p), profile_error);
        }
    }

    SECTION("output") {
        SECTION("valid") {
            std::string p = R"(
profiles:
  test:
    command: "create"
    options:
      output: "output.torrent"
)";
            GET_TEST_OPTIONS(p)
            CHECK(options.destination == fs::path("output.torrent"));
        }

        SECTION("valid - write to stdout") {
            std::string p = R"(
profiles:
  test:
    command: "create"
    options:
      output: "-"
)";
            GET_TEST_OPTIONS(p)
            CHECK_FALSE(options.destination.has_value());
            CHECK(options.write_to_stdout);
        }

        SECTION("bad type") {
            std::string p = R"(
profiles:
  test:
    command: "create"
    options:
      output: [ "test" ]
)";
            CHECK_THROWS_AS(config(p), profile_error);
        }
    }

    SECTION("piece-size") {
        SECTION("bad type") {
            std::string p = R"(
profiles:
  test:
    command: "create"
    options:
      piece-size: [ "test" ]
)";
            CHECK_THROWS_AS(config(p), profile_error);
        }
    }
    SECTION("private") {
        SECTION("bad type") {
            std::string p = R"(
profiles:
  test:
    command: "create"
    options:
      private: [ "test" ]
)";
            CHECK_THROWS_AS(config(p), profile_error);
        }
    }
    SECTION("protocol") {
        SECTION("bad type") {
            std::string p = R"(
profiles:
  test:
    command: "create"
    options:
      protocol: [ "test" ]
)";
            CHECK_THROWS_AS(config(p), profile_error);
        }
    }

    SECTION("set-created-by") {
        SECTION("bad type") {
            std::string p = R"(
profiles:
  test:
    command: "create"
    options:
      set-created-by: "test"
)";
            CHECK_THROWS_AS(config(p), profile_error);
        }
    }
    SECTION("set-creation-date") {
        SECTION("bad type") {
            std::string p = R"(
profiles:
  test:
    command: "create"
    options:
      set-creation-date: "test"
)";
            CHECK_THROWS_AS(config(p), profile_error);
        }
    }

    SECTION("similar") {
        SECTION("bad type") {
            std::string p = R"(
profiles:
  test:
    command: "create"
    options:
      similar: "test"
)";
            CHECK_THROWS_AS(config(p), profile_error);
        }
    }

    SECTION("source") {
        SECTION("bad type") {
            std::string p = R"(
profiles:
  test:
    command: "create"
    options:
      source: ["test"]
)";
            CHECK_THROWS_AS(config(p), profile_error);
        }
    }

    SECTION("threads") {
        SECTION("valid") {
            std::string p = R"(
profiles:
  test:
    command: "create"
    options:
      threads: 4
)";
            GET_TEST_OPTIONS(p);
            CHECK(options.threads==4);
        }

        SECTION("bad type") {
            std::string p = R"(
profiles:
  test:
    command: "create"
    options:
      threads: "test"
)";
            CHECK_THROWS_AS(config(p), profile_error);
        }
    }

    SECTION("web-seed") {
        SECTION("valid") {
            std::string p = R"(
profiles:
  test:
    command: "create"
    options:
      web-seed: [ "http://seed1.com:1234", "http://seed2.com:5678"]
)";
            GET_TEST_OPTIONS(p);
            CHECK(options.web_seeds == std::vector{ "http://seed1.com:1234"s, "http://seed2.com:5678"s});
        }
        SECTION("bad type") {
            std::string p = R"(
profiles:
  test:
    command: "create"
    options:
      web-seed: "test"
)";
            CHECK_THROWS_AS(config(p), profile_error);
        }
    }
}


