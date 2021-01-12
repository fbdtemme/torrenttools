#include <catch2/catch.hpp>
#include <fmt/format.h>
#include <experimental/source_location>
#include <CLI/CLI.hpp>

#include "create.hpp"


#define PARSE_ARGS(cmd) \
try { app.parse(cmd); } catch (CLI::ParseError& e) { FAIL(e.what()); } \


TEST_CASE("test create app argument parsing")
{
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
}