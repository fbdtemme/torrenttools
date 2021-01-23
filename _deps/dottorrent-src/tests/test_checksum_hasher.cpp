//
// Created by fbdtemme on 7/11/20.
//

#include <catch2/catch.hpp>

#include <filesystem>
#include <ranges>


#include <dottorrent/metafile.hpp>
#include <dottorrent/storage_hasher.hpp>
#include <dottorrent/file_storage.hpp>
#include <dottorrent/file_entry.hpp>

namespace dt = dottorrent;
namespace fs = std::filesystem;
namespace rng = std::ranges;

//
//TEST_CASE("per file checksum hasher - sha1")
//{
//    auto path = fs::path(TEST_DIR"/resources/torrent1");
//    auto m = dt::metafile{};
//    auto& s = m.storage();
//
//    s.set_root_directory(path);
//    s.add_files(fs::directory_iterator(path), fs::directory_iterator());
//
//    dt::storage_hasher hasher(s, {.checksums = {dt::hash_function::sha1}});
//    hasher.start();
//    hasher.wait();
//
//    auto& file = s[0];
//    auto checksum = file.get_checksum("sha1");
//    REQUIRE(checksum != nullptr);
//    REQUIRE(checksum->name() == "sha1");
//
//    auto control_checksum = dt::make_checksum_from_hex(
//            dt::hash_function::sha1, "d99e1bcc3e1539de63f5d2ece8df6c6662319329");
////    CHECK(rng::equal(checksum->value(), control_checksum->value()));
////    CHECK(checksum == control_checksum);
//}
