

#include <catch2/catch.hpp>

#include <dottorrent/merkle_tree.hpp>
#include <algorithm>
#include <string_view>
#include <bit>


TEST_CASE("Test merkle tree")
{
    using namespace dottorrent;
    using namespace Catch::Generators;

    constexpr std::size_t piece_count = 10;
    // Test tree for 10 pieces
    auto tree = merkle_tree<hash_function::sha256>{piece_count};

    SECTION("Construction") {
        // Verify the tree is complete
        CHECK(std::has_single_bit(tree.node_count() + 1));
        // values are zero-initialized
        auto& root = tree.root();
        CHECK(std::all_of(root.begin(), root.end(), [](auto v) { return v == std::byte(0); }));
    }

    SECTION("tree stats") {
        CHECK(tree.tree_height() == 4);
        CHECK(tree.node_count() == 31);
        CHECK(tree.leaf_count() == 16);
    }

    constexpr std::array<std::string_view, 10> pieces_data = {
            "aasc", "asasd", "igrrg", "dqw",
            "pf93", "92123f", "339jr", "q0vn4g",
            "d-2hg", "jkloklew"
    };

    SECTION("Add leave pieces") {
        auto hasher = make_hasher(hash_function::sha256);

        for (std::size_t i = 0; i < piece_count; ++i) {
            sha256_hash h {};
            hasher->update(pieces_data[i]);
            hasher->finalize_to(h);
            tree.set_leaf(i, h);
        }

        for (std::size_t i = 0; i < piece_count; ++i) {
            CHECK(tree.get_leaf(i) != sha256_hash{});
        }
    }
    SECTION("Calculate root") {
        tree.update();
        // root hash is actually set
        CHECK(tree.root() != sha256_hash{});
    }
}