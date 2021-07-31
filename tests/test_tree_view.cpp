//
// Created by fbdtemme on 2/26/20.
//
#include <filesystem>
#include <iostream>
#include <fstream>


#include <catch2/catch.hpp>

#include <dottorrent/metafile.hpp>
#include "tree_view.hpp"
#include "test_resources.hpp"

namespace fs = std::filesystem;
namespace dt = dottorrent;

TEST_CASE("test tree_index") {
    SECTION("only root dir") {
        std::ifstream ifs(fedora_torrent, std::ios::binary);
        auto m = dt::read_metafile(ifs);
        auto index = filetree_index(m.storage());
        auto root_size = index.get_directory_size("");

        SECTION("directory size of root dir") {
            CHECK(root_size == m.total_file_size());
        }
        SECTION("directory list for root dir") {
            auto c = index.list_directory_content("");
            CHECK(c[0].path == "Fedora-Workstation-30-1.2-x86_64-CHECKSUM");
            CHECK(c[1].path == "Fedora-Workstation-Live-x86_64-30-1.2.iso");
        }
    }

    SECTION ("nested file structure") {
        std::ifstream ifs(tree_index_test, std::ios::binary);
        auto m = dt::read_metafile(ifs);
        auto index = filetree_index(m.storage());
        auto root_size = index.get_directory_size("");

        SECTION("directory list for root dir") {
            auto c = index.list_directory_content("");
            SUCCEED();
        }

        SECTION("directory size of root dir") {
            CHECK(root_size == m.total_file_size());
        }

        SECTION("file list") {
            auto l = index.list_directory_content("dir1");
            CHECK(l.size() == 3);
            CHECK(l[0].path == "dir3");
            CHECK(l[1].path == "f1");
            CHECK(l[2].path == "f2");

            l = index.list_directory_content("dir2");
            CHECK(l.size() == 4);
            CHECK(l[0].path == "f1");
            CHECK(l[1].path == "f2");
            CHECK(l[2].path == "f3");
            CHECK(l[3].path == "f4");
        }
    }
}
