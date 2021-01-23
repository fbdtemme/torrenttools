#include <dottorrent/metafile.hpp>
#include <dottorrent/hash.hpp>
#include <dottorrent/storage_verifier.hpp>

#include <catch2/catch.hpp>
#include <iostream>
#include <ranges>


using namespace dottorrent;
using namespace std::string_view_literals;
namespace fs = std::filesystem;
namespace rng = std::ranges;


//TEST_CASE("verify torrent")
//{
////    fs::path torrent_path("/home/fbdtemme/Videos/Élite.S01.1080p.NF.WEB-DL.DDP5.1.x264-MZABI.torrent");
//    auto m = dottorrent::load_metafile(torrent_path);
//    auto& storage = m.storage();
//
////    storage.set_root_directory("/home/fbdtemme/Videos/Élite.S01.1080p.NF.WEB-DL.DDP5.1.x264-MZABI");
//    storage_verifier verifier(storage);
//    verifier.start();
//    verifier.wait();
//    CHECK(verifier.done());
//
//    auto result = verifier.result();
//    CHECK_FALSE(rng::all_of(result, [](auto& v) { return v == 1; }));
//    auto t = result;
//}