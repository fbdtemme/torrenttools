#include "dottorrent/v2_chunk_verifier.hpp"

namespace dottorrent {

v2_chunk_verifier::v2_chunk_verifier(file_storage& storage, std::size_t thread_count)
        : base_type(storage, {hash_function::sha256}, thread_count)
        , file_bytes_hashed_()
        , merkle_trees_()
        , piece_map_()
        , file_offsets_()
{
    // SHA265 hash of 16 KiB of zero bytes.
    std::size_t file_count = 0;
    std::size_t piece_size = storage.piece_size();

    Expects(piece_size >= v2_block_size);
    Expects(piece_size % v2_block_size == 0);

    file_offsets_.push_back(0);

    for (const auto& entry : storage) {
        ++file_count;
        auto block_count = (entry.file_size() + v2_block_size -1) / v2_block_size;
        if (entry.is_padding_file()) {
            // add en empty merkly tree to make sure file_indices match merkle tree indices.
            merkle_trees_.emplace_back();
            file_offsets_.emplace_back(std::numeric_limits<std::size_t>::max());
        }
        else {
            auto& m = merkle_trees_.emplace_back(block_count);
            auto offset = std::max(std::size_t(1), entry.piece_layer().size());
            file_offsets_.push_back(file_offsets_.back() + offset);
        }
    }
    file_bytes_hashed_ = decltype(file_bytes_hashed_)(file_count);

    auto piece_map_size = 0;
    for (const auto& entry : storage) {
        auto s = entry.piece_layer().size();
        if (s != 0) {
            piece_map_size += s;
        } else {
            // we have a file smaller then the piece size with only a root hash.
            piece_map_size += 1;
        }
    }
    piece_map_.resize(piece_map_size);
}

void v2_chunk_verifier::hash_chunk(std::vector<std::unique_ptr<hasher>>& hashers, const data_chunk& chunk)
{
    hash_chunk(*hashers.front(), chunk);
}


void v2_chunk_verifier::hash_chunk(hasher& sha256_hasher, const data_chunk& chunk)
{
    file_storage& storage = storage_.get();

    Expects(chunk.file_index < storage.file_count());

    // Piece without any data indicate a missing file.
    // We do not upgrade bytes hashed but mark the whole file as done.
    if (chunk.data == nullptr) {
        auto file_size = storage[chunk.file_index].file_size();
        bytes_done_.fetch_add(file_size, std::memory_order_relaxed);
        return;
    }

    file_entry& entry = storage[chunk.file_index];
    auto& tree = merkle_trees_[chunk.file_index];
    auto& file_progress = file_bytes_hashed_[chunk.file_index];
    const auto piece_size = storage.piece_size();
    const auto pieces_in_chunk = (chunk.data->size() + piece_size - 1) / piece_size;
    // number of 16 KiB blocks in a chunk
    const auto blocks_in_chunk = (chunk.data->size() + v2_block_size -1) / v2_block_size;
    // index of first 16 KiB block in the per file merkle tree
    const auto index_offset = chunk.piece_index * piece_size / v2_block_size;
    const auto data = std::span(*chunk.data);

    sha256_hash leaf {};

    std::size_t i = 0;
    for ( ; i < blocks_in_chunk-1; ++i) {
        sha256_hasher.update(data.subspan(i * v2_block_size, v2_block_size));
        sha256_hasher.finalize_to(leaf);
        tree.set_leaf(index_offset + i, leaf);
        bytes_hashed_.fetch_add(v2_block_size);
    }

    // last block can be smaller then the block size!
    auto final_block = data.subspan(i * v2_block_size);
    sha256_hasher.update(final_block);
    sha256_hasher.finalize_to(leaf);
    bytes_hashed_.fetch_add(final_block.size());
    tree.set_leaf(index_offset + i, leaf);

    // Update per file progress and check if this thread did just finish the last chunk of
    // this file. Make sure to propagate memory effects so set_piece_layers sees all
    // leaf nodes of the merkle tree.
    auto tmp = file_progress.fetch_add(chunk.data->size(), std::memory_order_acq_rel);
    if (tmp == (entry.file_size() - chunk.data->size())) [[unlikely]] {
        verify_piece_layers_and_root(sha256_hasher, chunk.file_index);
    }

    bytes_done_.fetch_add(chunk.data->size(), std::memory_order_relaxed);
}


void v2_chunk_verifier::verify_piece_layers_and_root(hasher& hasher, std::size_t file_index)
{
    file_storage& storage = storage_;
    std::size_t piece_size = storage.piece_size();

    auto& tree = merkle_trees_[file_index];

    // complete the merkle tree
    tree.update(hasher);

    // the depth in the tree of hashes covering blocks of size `piece_size`
    auto layer_offset = detail::log2_floor(piece_size) - detail::log2_floor(v2_block_size);

    // verify the root hash
    file_entry& entry = storage.at(file_index);
    std::size_t entry_index = file_offsets_.at(file_index);

    bool whole_file_complete = entry.pieces_root() == tree.root();
    bool is_single_piece_file = entry.piece_layer().size() == 0;

    if (whole_file_complete) {
        if (is_single_piece_file) {
            piece_map_[entry_index] = 1;
        }
        else {
            std::fill_n(std::next(piece_map_.begin(), entry_index), entry.piece_layer().size(), 1);
        }
    }
    else {
        if (is_single_piece_file) {
            piece_map_[entry_index] = 0;
        }
        else {

            // leaf nodes necessary to balance the tree are not included in the piece layers
            std::size_t layer_depth = tree.tree_height() - layer_offset;
            auto layer_view = tree.get_layer(layer_depth);
            auto layer_data_nodes_size = (entry.file_size() + piece_size - 1 ) / piece_size;
            layer_view = layer_view.subspan(0, layer_data_nodes_size);
            auto reference_layer_view = entry.piece_layer();

            std::size_t max_offset = std::min(layer_view.size(), reference_layer_view.size());

            for (std::size_t i = 0; i < max_offset; ++i, ++entry_index) {
                piece_map_[entry_index] = (layer_view[i] == reference_layer_view[i]);
            }
        }
    }
}

const std::vector<std::uint8_t>& v2_chunk_verifier::result() const noexcept
{
    return piece_map_;
}

const double v2_chunk_verifier::percentage(std::size_t file_index) const noexcept
{
    Expects(file_index < file_offsets_.size());
    constexpr std::size_t max_size_t = std::numeric_limits<std::size_t>::max();

    const file_storage& storage = storage_;
    const auto& entry = storage[file_index];

    if (entry.is_padding_file())
        return 100;

    std::size_t number_of_pieces = 1;
    if (auto s = entry.piece_layer().size(); s > 0) {
        number_of_pieces = s;
    }

    auto first_piece_map_index = std::transform_reduce(
            storage.begin(), std::next(storage.begin(), file_index),
            0ul, std::plus<>{},
            [](const file_entry& e) { return std::max(std::size_t(1), e.piece_layer().size()); });

    auto n_complete_pieces = std::count(
            std::next(piece_map_.begin(), first_piece_map_index),
            std::next(piece_map_.begin(), first_piece_map_index+number_of_pieces), 1);
    return double(n_complete_pieces) / double(number_of_pieces) * 100;
}

} // namespace dottorrent