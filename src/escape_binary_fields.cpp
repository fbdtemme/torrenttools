#include "escape_binary_fields.hpp"
#include <dottorrent/hex.hpp>
#include <bencode/traits/span.hpp>


bc::bvalue escape_binary_metafile_fields(const bencode::bvalue& value)
{
    using namespace bencode::literals;

    bc::bvalue bv = value;
    bool has_v1_pieces = bv.contains("/info/pieces"_bpointer);
    bool has_v2_piece_layers = bv.contains("piece layers");


    if (has_v1_pieces) {
        auto& ref = bv.at("/info/pieces"_bpointer);
        auto num_pieces = (get_string(ref).size() + dt::sha1_hash::size_bytes-1) / dt::sha1_hash::size_bytes;
        ref = fmt::format("<{} piece hashes>", num_pieces);
    }

    if (has_v2_piece_layers) {
        auto cleaned_dict = bc::bvalue::dict_type{};
        auto& layers_bv = bv.at("piece layers");
        for (auto& [k, v] : get_dict(layers_bv)) {
            auto key = fmt::format("<merkle root {}>", dt::sha256_hash(k).hex_string());
            cleaned_dict[key] = fmt::format("<{} piece hashes>", get_string(v).size() / dt::sha256_hash::size());
        }
        layers_bv = std::move(cleaned_dict);

        auto& file_tree_bv = bv.at("/info/file tree"_bpointer);
        auto& file_tree_dict = get_dict(file_tree_bv);
        // do not use recursion to be safe from stack overflow for very deep trees.
        using frame_type = std::pair<
                std::reference_wrapper<typename bc::bvalue::dict_type>,
                typename bc::bvalue::dict_iterator >;

        std::stack<frame_type> frames {};
        frames.push(frame_type(file_tree_dict, file_tree_dict.begin()));

        while (!frames.empty()) {
            auto& [dict_ref, it] = frames.top();
            auto& dict = dict_ref.get();

            if (it != rng::end(dict)) {
                const auto& key = it->first;
                auto& value = bc::get_dict(it->second);

                // leaf node : a file
                if (key.empty()) {
                    auto& root = get_string(value.at("pieces root"));
                    root = fmt::format("<merkle root {}>", dt::sha256_hash(root).hex_string());
                }
                else {
                    frames.push(frame_type(value, value.begin()));
                }
                ++it;
            } else {
                frames.pop();
            }
        }
    }

    return bv;
}


inline auto make_v1_piece_list(bc::bvalue& pieces_field)
{
    auto byte_view = get_as<std::span<const std::byte>>(pieces_field);
    // round up to handle single piece torrents
    auto num_pieces = (byte_view.size() + dt::sha1_hash::size_bytes-1) / dt::sha1_hash::size_bytes;
    auto hex_data = dt::to_hexadecimal_string(byte_view);

    constexpr auto hex_piece_length = 2 * dt::sha1_hash::size_bytes;
    pieces_field.emplace_list();

    for (std::size_t i = 0; i < num_pieces; ++i) {
        auto piece = std::string_view(hex_data).substr(hex_piece_length*i, hex_piece_length);
        pieces_field.push_back(fmt::format("<piece: {}, SHA1: {}>", i, piece));
    }
}

inline auto make_v2_piece_list(bc::bvalue& pieces_field)
{
    auto byte_view = get_as<std::span<const std::byte>>(pieces_field);
    // round up to handle single piece torrents
    auto num_pieces = (byte_view.size() + dt::sha256_hash::size_bytes-1) / dt::sha256_hash::size_bytes;
    auto hex_data = dt::to_hexadecimal_string(byte_view);

    constexpr auto hex_piece_length = 2 * dt::sha1_hash::size_bytes;
    pieces_field.emplace_list();

    for (std::size_t i = 0; i < num_pieces; ++i) {
        auto piece = std::string_view(hex_data).substr(hex_piece_length*i, hex_piece_length);
        pieces_field.push_back(fmt::format("<piece: {}, SHA256: {}>", i, piece));
    }
}


bc::bvalue escape_binary_metafile_fields_hex(const bencode::bvalue& value)
{
    using namespace bencode::literals;

    bc::bvalue bv = value;
    bool has_v1_pieces = bv.contains("/info/pieces"_bpointer);
    bool has_v2_piece_layers = bv.contains("piece layers");

    if (has_v1_pieces) {
        auto& ref = bv.at("/info/pieces"_bpointer);
        make_v1_piece_list(ref);
    }

    if (has_v2_piece_layers) {
        auto cleaned_dict = bc::bvalue::dict_type{};
        auto& layers_bv = bv.at("piece layers");
        for (auto& [k, v] : get_dict(layers_bv)) {
            auto key = fmt::format("<merkle root {}>", dt::sha256_hash(k).hex_string());
            auto num_pieces = get_string(v).size() / dt::sha256_hash::size();
            make_v2_piece_list(v);
            cleaned_dict[key] = std::move(v);
        }
        layers_bv = std::move(cleaned_dict);

        auto& file_tree_bv = bv.at("/info/file tree"_bpointer);
        auto& file_tree_dict = get_dict(file_tree_bv);
        // do not use recursion to be safe from stack overflow for very deep trees.
        using frame_type = std::pair<
                std::reference_wrapper<typename bc::bvalue::dict_type>,
                typename bc::bvalue::dict_iterator >;

        std::stack<frame_type> frames {};
        frames.push(frame_type(file_tree_dict, file_tree_dict.begin()));

        while (!frames.empty()) {
            auto& [dict_ref, it] = frames.top();
            auto& dict = dict_ref.get();

            if (it != rng::end(dict)) {
                const auto& key = it->first;
                auto& value = bc::get_dict(it->second);

                // leaf node : a file
                if (key.empty()) {
                    auto& root = get_string(value.at("pieces root"));
                    root = fmt::format("<merkle root {}>", dt::sha256_hash(root).hex_string());
                }
                else {
                    frames.push(frame_type(value, value.begin()));
                }
                ++it;
            } else {
                frames.pop();
            }
        }
    }

    return bv;
}