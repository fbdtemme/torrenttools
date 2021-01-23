//
// Created by fbdtemme on 20/01/19.
//
#include <iostream>
#include <numeric>

#include <catch2/catch.hpp>
#include <dottorrent/metafile.hpp>
#include <bencode/bencode.hpp>

using namespace dottorrent;


const auto tracker1 = "http://tracker1.com";
const auto tracker2 = "http://tracker2.com";
const auto tracker3 = "http://tracker3.com";
const auto tracker4 = "http://tracker4.com";


TEST_CASE("test add_tracker", "[metafile]") {
    metafile torrent{};

    SECTION("add a tracker with default tier") {
        torrent.add_tracker(tracker1);
        const auto entry = torrent.trackers().begin();
        REQUIRE(entry->url == tracker1);
        REQUIRE(entry->tier == 0);
    }

    SECTION("add a tracker with identical url but different tier") {
        torrent.add_tracker(tracker1);
        torrent.add_tracker(tracker2, 1);
        torrent.add_tracker(tracker1, 1);
        auto trackers = torrent.trackers();
        CHECK(trackers[0] == announce_url(tracker1, 0));
        CHECK(trackers[1] == announce_url(tracker2, 0));
    }

    SECTION("split tier in two different tiers") {
        torrent.add_tracker(tracker1);
        torrent.add_tracker(tracker2);
        torrent.add_tracker(tracker2, 1);
        auto trackers = torrent.trackers();
        CHECK(trackers[0] == announce_url(tracker1, 0));
        CHECK(trackers[1] == announce_url(tracker2, 1));
    }
}

TEST_CASE("test remove_tracker", "[metafile]") {
    metafile torrent {};
    torrent.add_tracker(tracker1, 0);
    torrent.add_tracker(tracker2, 1);
    torrent.add_tracker(tracker3, 2);
    torrent.add_tracker(tracker4, 2);

    SECTION("remove last of tier tracker tracker") {
        torrent.remove_tracker(tracker1);
        const auto trackers = torrent.trackers();
        CHECK(trackers[0] == announce_url(tracker2, 0));
    }

    SECTION("remove last of tier tracker tracker of tier 1") {
        torrent.remove_tracker(tracker2);
        const auto trackers = torrent.trackers();
        CHECK(trackers[1].url == tracker3);
        CHECK(trackers[1].tier == 1);
    }
    SECTION("remove non last of tier tracker tracker") {
        torrent.remove_tracker(tracker3);
        const auto trackers = torrent.trackers();
        CHECK(trackers[2].url == tracker4);
        CHECK(trackers[2].tier == 2);
    }
}

TEST_CASE("test clear_trackers", "[metafile]") {
    metafile torrent {};
    torrent.add_tracker(tracker1, 0);
    torrent.add_tracker(tracker2, 1);
    torrent.add_tracker(tracker3, 2);
    torrent.add_tracker(tracker4, 2);

    torrent.clear_trackers();
    CHECK(torrent.trackers().empty());
}

