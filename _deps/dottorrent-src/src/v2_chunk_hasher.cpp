//
// Created by fbdtemme on 9/22/20.
//
#include "dottorrent/v2_chunk_hasher.hpp"

namespace dottorrent {

v2_chunk_hasher::v2_chunk_hasher(file_storage& storage, std::size_t capacity, bool v1_compatible, std::size_t thread_count)
        : base_type(storage, {hash_function::sha1, hash_function::sha256}, capacity, thread_count)
        , piece_size_(storage.piece_size())
        , file_bytes_hashed_()
        , add_v1_compatibility_(v1_compatible)
{
    // SHA265 hash of 16 KiB of zero bytes.
    const auto& st = storage_.get();
    std::size_t file_count = 0;

    v1_piece_offsets_.push_back(0);

    for (const auto& entry : storage) {
        ++file_count;
        auto block_count = (entry.file_size() + v2_block_size -1) / v2_block_size;
        if (entry.is_padding_file()) {
            v1_piece_offsets_.push_back(v1_piece_offsets_.back());
            // add en empty merkly tree to make sure file_indices match merkle tree indices.
            merkle_trees_.emplace_back();
        }
        else {
            merkle_trees_.emplace_back(block_count);
            v1_piece_offsets_.push_back(
                    v1_piece_offsets_.back()+(entry.file_size()+piece_size_-1)/piece_size_);
        }
    }
    file_bytes_hashed_ = decltype(file_bytes_hashed_)(file_count);
}

void v2_chunk_hasher::hash_chunk(std::vector<std::unique_ptr<hasher>>& hashers, const data_chunk& chunk)
{
    hash_chunk(*hashers.back(), *hashers.front(), chunk);
}


void v2_chunk_hasher::hash_chunk(hasher& sha256_hasher, hasher& sha1_hasher, const data_chunk& chunk)
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
    const auto index_offset = chunk.piece_index * piece_size_ / v2_block_size;
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
        set_piece_layers_and_root(sha256_hasher, chunk.file_index);
    }

    if (add_v1_compatibility_) {
        // v1 compatibility data
        Expects(pieces_in_chunk >= 1);
        sha1_hash piece_hash{};

        bool needs_padding = chunk.data->size() % piece_size != 0;
        std::size_t pieces_to_process = 0;

        // process the complete pieces of the chunk
        if  (!needs_padding) {
            pieces_to_process = pieces_in_chunk;
        } else {
            // last piece needs padding or is the last piece of the file
            pieces_to_process = pieces_in_chunk -1;
        }

        std::size_t piece_in_chunk_index = 0;
        for (; piece_in_chunk_index < pieces_to_process; ++piece_in_chunk_index)
        {
            sha1_hasher.update(data.subspan(piece_size * piece_in_chunk_index, piece_size));
            sha1_hasher.finalize_to(piece_hash);
            process_piece_hash(chunk.piece_index + piece_in_chunk_index, chunk.file_index, piece_hash);
            bytes_hashed_.fetch_add(piece_size, std::memory_order::relaxed);
        }

        // we have an incomplete final piece so we have the last piece of a file.
        // we need to pad with zeros in case it is not the last file in the torrent
        if (needs_padding) {
            if (chunk.file_index+1 < storage.file_count()-1) {
                const auto& entry = storage.at(chunk.file_index+1);
                Expects(entry.is_padding_file());
                padding_.resize(entry.file_size(), std::byte{0});

                // add the final partial piece and pad the rest of the piece
                auto final_piece = data.subspan(piece_in_chunk_index * piece_size);
                sha1_hasher.update(final_piece);
                sha1_hasher.update(padding_);
                sha1_hasher.finalize_to(piece_hash);
                process_piece_hash(chunk.piece_index + piece_in_chunk_index, chunk.file_index, piece_hash);
                bytes_hashed_.fetch_add(final_piece.size() + padding_.size(), std::memory_order::relaxed);
            }
            else {
                auto final_piece = data.subspan(piece_in_chunk_index * piece_size);
                sha1_hasher.update(final_piece);
                sha1_hasher.finalize_to(piece_hash);
                process_piece_hash(chunk.piece_index + piece_in_chunk_index, chunk.file_index, piece_hash);
                bytes_hashed_.fetch_add(final_piece.size(), std::memory_order::relaxed);
            }
        }
    }
    bytes_done_.fetch_add(chunk.data->size(), std::memory_order_relaxed);
}


void v2_chunk_hasher::set_piece_layers_and_root(hasher& hasher, std::size_t file_index) {
    Ensures(piece_size_ >= v2_block_size);
    Ensures(piece_size_ % v2_block_size == 0);

    file_storage& storage = storage_;
    auto& tree = merkle_trees_[file_index];

    // complete the merkle tree
    tree.update(hasher);

    // the depth in the tree of hashes covering blocks of size `piece_size`
    auto layer_offset = detail::log2_floor(piece_size_) - detail::log2_floor(v2_block_size);

    // set root hash
    file_entry& entry = storage.at(file_index);
    entry.set_pieces_root(tree.root());

    // files smaller then piece size have empty piece_layers
    if (layer_offset >= tree.tree_height()) {
        entry.set_piece_layer({});
        return;
    }

    std::size_t layer_depth = tree.tree_height() - layer_offset;

    // leaf nodes necessary to balance the tree are not included in the piece layers
    auto layer_view = tree.get_layer(layer_depth);
    auto layer_data_nodes_size = (entry.file_size() + piece_size_ - 1 ) / piece_size_;

    if (layer_view.size() != layer_data_nodes_size) {
        layer_view = layer_view.subspan(0, layer_data_nodes_size);
    }

    entry.set_piece_layer(layer_view);
}

void v2_chunk_hasher::process_piece_hash(std::size_t piece_idx, std::size_t file_index, const sha1_hash& piece_hash)
{
    auto global_piece_index = v1_piece_offsets_[file_index] + piece_idx;
    file_storage& storage = storage_;
    storage.set_piece_hash(global_piece_index, piece_hash);
}



}