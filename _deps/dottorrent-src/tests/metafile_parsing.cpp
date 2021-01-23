#include <fstream>
#include <filesystem>
#include <catch2/catch.hpp>

#include <chrono>

#include <bencode/bencode.hpp>
#include <dottorrent/metafile.hpp>
#include "dottorrent/metafile_parsing.hpp"

namespace fs = std::filesystem;
using namespace std::chrono_literals;

struct torrent_fixture
{
    torrent_fixture(fs::path path)
    {
        auto is = std::ifstream(path, std::ifstream::binary);
        buffer = std::string(std::istreambuf_iterator<char>{is},
                std::istreambuf_iterator<char>{});
        descriptors = bencode::decode_view(buffer);
        bview = descriptors.get_root();
        bvalue = bencode::bvalue(bview);
    }

    std::string buffer {};
    bencode::descriptor_table descriptors {};
    bencode::bview bview {};
    bencode::bvalue bvalue {};
};

static auto fedora_torrent = torrent_fixture(
        TEST_DIR"/resources/Fedora-Workstation-Live-x86_64-30.torrent");

static auto v2_torrent = torrent_fixture(
        TEST_DIR"/resources/bittorrent-v2-test.torrent");

static auto hybrid_torrent = torrent_fixture(
        TEST_DIR"/resources/bittorrent-v2-hybrid-test.torrent");

static auto rsna_torrent = torrent_fixture(
                TEST_DIR"/resources/RSNA_Pneumonia_Detection_Challenge.torrent");



TEST_CASE("parse announce urls", "[metafile]")
{
    dottorrent::metafile m {};

    SECTION("bvalue") {
        dottorrent::detail::parse_announce(fedora_torrent.bvalue, m);
        auto r = m.trackers();

        CHECK_FALSE(r.empty());
        CHECK(m.trackers()[0].url == "http://torrent.fedoraproject.org:6969/announce");
    }

    SECTION("bview") {
        dottorrent::detail::parse_announce(fedora_torrent.bview, m);
        auto r = m.trackers();

        CHECK_FALSE(r.empty());
        CHECK(r[0].url == "http://torrent.fedoraproject.org:6969/announce");
    }

    SECTION("rsna") {
        dottorrent::detail::parse_announce(rsna_torrent.bview, m);
        auto r = m.trackers();

        CHECK_FALSE(r.empty());
    }
}

TEST_CASE("parse private flag", "[metafile]")
{
    auto p = fs::path(TEST_DIR"/resources/Fedora-Workstation-Live-x86_64-30.torrent");
    dottorrent::metafile m {};

    SECTION("bvalue") {
        dottorrent::detail::parse_private(fedora_torrent.bvalue, m);
        CHECK_FALSE(m.is_private());
    }

    SECTION("bview") {
        dottorrent::detail::parse_announce(fedora_torrent.bview, m);
        auto r = m.trackers();

        CHECK_FALSE(m.is_private());
    }
}

TEST_CASE("parse comment", "[metafile]")
{
    auto p = fs::path(TEST_DIR"/resources/Fedora-Workstation-Live-x86_64-30.torrent");
    dottorrent::metafile m {};

    SECTION("bvalue") {
        dottorrent::detail::parse_comment(fedora_torrent.bvalue, m);
        CHECK(m.comment().empty());
    }

    SECTION("bview") {
        dottorrent::detail::parse_comment(fedora_torrent.bview, m);
        auto r = m.trackers();

        CHECK(m.comment().empty());
    }
}

TEST_CASE("parse created_by", "[metafile]")
{
    auto p = fs::path(TEST_DIR"/resources/Fedora-Workstation-Live-x86_64-30.torrent");
    dottorrent::metafile m {};

    SECTION("bvalue") {
        dottorrent::detail::parse_created_by(fedora_torrent.bvalue, m);
        CHECK(m.created_by().empty());
    }

    SECTION("bview") {
        dottorrent::detail::parse_created_by(fedora_torrent.bview, m);
        CHECK(m.created_by().empty());
    }
}

TEST_CASE("parse collections")
{
    dottorrent::metafile m {};

    SECTION("bvalue") {
        dottorrent::detail::parse_collections(fedora_torrent.bvalue, m);
        CHECK(m.collections().empty());
    }

    SECTION("bview") {
        dottorrent::detail::parse_collections(fedora_torrent.bview, m);
        CHECK(m.collections().empty());
    }
}


TEST_CASE("parse http seeds")
{
    dottorrent::metafile m {};

    SECTION("bvalue") {
        dottorrent::detail::parse_http_seeds(fedora_torrent.bvalue, m);
        CHECK(m.http_seeds().empty());
    }

    SECTION("bview") {
        dottorrent::detail::parse_http_seeds(fedora_torrent.bview, m);
        CHECK(m.http_seeds().empty());
    }
}

TEST_CASE("parse web seeds")
{
    dottorrent::metafile m {};

    SECTION("bvalue") {
        dottorrent::detail::parse_web_seeds(fedora_torrent.bvalue, m);
        CHECK(m.web_seeds().empty());
    }

    SECTION("bview") {
        dottorrent::detail::parse_web_seeds(fedora_torrent.bview, m);
        CHECK(m.web_seeds().empty());
    }
}

TEST_CASE("parse dht nodes")
{
    dottorrent::metafile m {};

    SECTION("bvalue") {
        dottorrent::detail::parse_dht_nodes(fedora_torrent.bvalue, m);
        CHECK(m.web_seeds().empty());
    }

    SECTION("bview") {
        dottorrent::detail::parse_dht_nodes(fedora_torrent.bview, m);
        CHECK(m.web_seeds().empty());
    }
}

TEST_CASE("parse creation date")
{
    dottorrent::metafile m {};

    SECTION("bvalue") {
        dottorrent::detail::parse_creation_date(fedora_torrent.bvalue, m);
        CHECK(m.creation_date() == std::chrono::seconds(1556547852));
    }

    SECTION("bview") {
        dottorrent::detail::parse_creation_date(fedora_torrent.bview, m);
        CHECK(m.web_seeds().empty());
    }
}

TEST_CASE("parse protocol version")
{
    dottorrent::metafile m {};

    SECTION("v1 torrent") {
        SECTION("bvalue") {
            auto p = dottorrent::detail::parse_protocol(fedora_torrent.bvalue);
            CHECK(p==dottorrent::protocol::v1);
        }
        SECTION("bview") {
            auto p = dottorrent::detail::parse_protocol(fedora_torrent.bview);
            CHECK(p==dottorrent::protocol::v1);
        }
    }

    SECTION("v2 torrent") {
        SECTION("bvalue") {
            auto p = dottorrent::detail::parse_protocol(v2_torrent.bvalue);
            CHECK(p==dottorrent::protocol::v2);
        }
        SECTION("bview") {
            auto p = dottorrent::detail::parse_protocol(v2_torrent.bview);
            CHECK(p==dottorrent::protocol::v2);
        }
    }

    SECTION("hybrid torrent") {
        SECTION("bvalue") {
            auto p = dottorrent::detail::parse_protocol(hybrid_torrent.bvalue);
            CHECK(p == dottorrent::protocol::v2);
        }
        SECTION("bview") {
            auto p = dottorrent::detail::parse_protocol(hybrid_torrent.bview);
            CHECK(p == dottorrent::protocol::v2);
        }
    }
}

TEST_CASE("parse v2 file tree")
{
    SECTION("v2 torrent") {
        dottorrent::metafile m{};

        SECTION("bvalue") {
            dottorrent::detail::parse_file_tree_v2(v2_torrent.bvalue, m);
            CHECK(m.storage().at(0).pieces_root()!=dottorrent::sha256_hash{});
        }
        SECTION("bview") {
            dottorrent::detail::parse_file_tree_v2(v2_torrent.bview, m);
        }
    }
}

TEST_CASE("parse v2 piece layers")
{
    SECTION("v2 torrent") {
        dottorrent::metafile m{};

        SECTION("bvalue") {
            dottorrent::detail::parse_file_tree_v2(v2_torrent.bvalue, m);
            dottorrent::detail::parse_piece_layers_v2(v2_torrent.bvalue, m);
            CHECK(!m.storage().at(0).piece_layer().empty());

            auto& storage = m.storage();
            const auto& file = storage.at(0);
            CHECK(file.has_v2_data());
        }
        SECTION("bview") {
            dottorrent::detail::parse_file_tree_v2(v2_torrent.bvalue, m);
            dottorrent::detail::parse_piece_layers_v2(v2_torrent.bview, m);
            CHECK(!m.storage().at(0).piece_layer().empty());

            auto& storage = m.storage();
            const auto& file = storage.at(0);
            CHECK(file.has_v2_data());
        }
    }
}