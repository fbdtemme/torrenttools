
#include <catch2/catch.hpp>
#include "tracker_database.hpp"

TEST_CASE("test trackers")
{
    using namespace torrenttools;
    auto db_path = TEST_DIR"/../resources/trackers.json";

    auto db = torrenttools::tracker_database(db_path);

    SECTION("find_by_name") {
        SECTION("valid name") {
            auto it = db.find_by_name("broadcastthenet");
            CHECK(it != db.end());
            CHECK(it->name == "BroadcastTheNet");
        }
        SECTION("invalid name") {
            auto it = db.find_by_name("qwdqwdqwd");
            CHECK(it == db.end());
        }
    }

    SECTION("find_by_abbreviation") {
        SECTION("valid abbreviation") {
            auto it = db.find_by_abbreviation("BTN");
            CHECK(it != db.end());
            CHECK(it->abbreviation == "BTN");
        }
        SECTION("invalid abbreviation") {
            auto it = db.find_by_abbreviation("qwdqwdqwd");
            CHECK(it == db.end());
        }
    }

    SECTION("find_by_url") {
        SECTION("valid abbreviation") {
            auto it = db.find_by_url("http://landof.tv/132qpm29f201m2p0vjk1/announce");
            CHECK(it != db.end());
            CHECK(it->abbreviation == "BTN");
        }
        SECTION("invalid abbreviation") {
            auto it = db.find_by_url("qwdqwdqwd");
            CHECK(it == db.end());
        }
    }

    SECTION("lookup name") {
        const auto& entry = db.at("broadcastthenet");
        CHECK(entry.name == "BroadcastTheNet");
        CHECK(entry.abbreviation == "BTN");
    }
}