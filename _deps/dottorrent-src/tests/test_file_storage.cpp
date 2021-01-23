////
//// Created by fbdtemme on 11/02/19.
////
//
#include <catch2/catch.hpp>
#include <iostream>

#include "dottorrent/file_entry.hpp"
#include "dottorrent/file_storage.hpp"


TEST_CASE("optimize_alignment", "[storage]") {
    using namespace dottorrent::literals;
    using namespace dottorrent;
    file_storage storage {};

    storage.add_file(file_entry{"test1", 2_MiB});
    storage.add_file(file_entry{"test2", 123_KiB});
    storage.add_file(file_entry{"test3", 3_KiB});
    storage.add_file(file_entry{"test4", 18_KiB});

    storage.set_piece_size(1_MiB);
    optimize_alignment(storage);

    CHECK(storage.size() == 6);
}

TEST_CASE("is_piece_aligned", "[storage]")
{
    using namespace dottorrent::literals;
    using namespace dottorrent;
    file_storage storage {};

    storage.set_piece_size(1_MiB);
    storage.add_file(file_entry{"test1", 2_MiB});
    storage.add_file(file_entry{"test2", 123_KiB});
    storage.add_file(file_entry{"test3", 3_KiB});

    CHECK_FALSE(is_piece_aligned(storage));
    optimize_alignment(storage);
    CHECK(is_piece_aligned(storage));
}