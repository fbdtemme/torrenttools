#include "dottorrent/metafile.hpp"
#include "dottorrent/metafile_serialization.hpp"

#include <bencode/encode.hpp>
#include <bencode/bvalue.hpp>
#include <bencode/traits/span.hpp>


namespace dottorrent::detail {

bencode::bvalue make_bvalue_infodict_v1(const metafile& m)
{
    bencode::bvalue binfo(bc::btype::dict);
    auto& info = get_dict(binfo);
    const auto& storage = m.storage();

    if (storage.file_mode() == file_mode::single) {
        auto& file = storage.at(0);
        info.insert_or_assign("length", file.file_size());
        info.insert_or_assign("name", file.path().filename().string());
    }
    else if (storage.file_mode() == file_mode::multi) {
        bencode::bvalue file_list(bc::btype::list);
        auto& bfile_list = get_list(file_list);
        bfile_list.reserve(storage.file_count());

        for (auto& file : storage) {
            auto file_info = bc::bvalue(bc::btype::dict);
            file_info["length"] = file.file_size();
            file_info["path"] = file.path();

            if (file.attributes()) {
                file_info["attr"] = file.attributes().value();
            }
            if (file.is_symlink()) {
                file_info["symlink path"] = file.symlink_path().value();
            }
            for (const auto& [algo, checksum] : file.checksums()) {
                file_info[algo] = checksum->value();
            }
            bfile_list.push_back(std::move(file_info));
        }
        info.insert_or_assign("files", std::move(file_list));

        if (m.name().empty()) {
            info.insert_or_assign("name", storage.root_directory().filename().string());
        }
        else {
            info.insert_or_assign("name", m.name());
        }
    }

    info.insert_or_assign("piece length", storage.piece_size());
    info.insert_or_assign("pieces", make_v1_pieces_string(storage));

    // only add private fields if it is true to minimize size
    if (auto is_private = m.is_private(); is_private) {
        info.insert_or_assign("private", is_private);
    }

    return binfo;
}



bencode::bvalue make_bvalue_infodict_v2(const metafile& m)
{

    bencode::bvalue binfo(bc::btype::dict);
    auto& info = get_dict(binfo);
    const auto& storage = m.storage();

    if (m.name().empty()) {
        info.insert_or_assign("name", storage.root_directory().filename().string());
    }
    else {
        info.insert_or_assign("name", m.name());
    }
    info.insert_or_assign("meta version", 2);
    info.insert_or_assign("piece length", storage.piece_size());

    // /info/file tree

    bencode::bvalue bfile_tree(bc::btype::dict);
    auto& file_tree = get_dict(bfile_tree);

    auto ptr = &file_tree;
    for (const auto& file : storage) {
        for (const auto& component : file.path()) {
            if (auto it = ptr->find(component); it != ptr->end()) {
                ptr = &get_dict(it->second);
            }
            else {
                auto [it2, suc] = ptr->insert_or_assign(component.string(), bc::bvalue(bc::btype::dict));
                ptr = &get_dict(it2->second);
            }
        }

        auto file_info = bc::bvalue(bc::btype::dict);

        if (file.attributes()) {
            file_info["attr"] = file.attributes().value();
        }

        file_info["length"] = file.file_size();
        file_info["pieces root"] = file.pieces_root();

        if (file.is_symlink()) {
            file_info["symlink path"] = file.symlink_path().value();
        }

        for (const auto& [algo, checksum] : file.checksums()) {
            file_info[algo] = checksum->value();
        }

        ptr->insert_or_assign("", file_info);
        // reset ptr to root of file tree
        ptr = &file_tree;
    }

    info.insert_or_assign("file tree", std::move(bfile_tree));

    // only add private fields if it is true to minimize size
    if (auto is_private = m.is_private(); is_private) {
        info.insert_or_assign("private", is_private);
    }

    return binfo;
}

bencode::bvalue make_bvalue_infodict_hybrid(const metafile& m)
{
    bencode::bvalue binfo(bc::btype::dict);
    auto& info = get_dict(binfo);
    const auto& storage = m.storage();

    // v1
    if (storage.file_mode() == file_mode::single) {
        auto& file = storage[0];
        info.insert_or_assign("length", file.file_size());
        info.insert_or_assign("name", file.path().filename().string());
    }
    else if (storage.file_mode() == file_mode::multi) {
        bencode::bvalue file_list(bc::btype::list);
        auto& bfile_list = get_list(file_list);
        bfile_list.reserve(storage.file_count());

        for (auto& file : storage) {
            auto file_info = bc::bvalue(bc::btype::dict);
            file_info["length"] = file.file_size();
            file_info["path"] = file.path();

            if (file.attributes()) {
                file_info["attr"] = file.attributes().value();
            }

            if (file.is_symlink()) {
                file_info["symlink path"] = file.symlink_path().value();
            }

            for (const auto& [algo, checksum] : file.checksums()) {
                file_info[algo] = checksum->value();
            }

            bfile_list.push_back(std::move(file_info));
        }
        info.insert_or_assign("files", std::move(file_list));
    }
        // /info/file tree

    bencode::bvalue bfile_tree(bc::btype::dict);
    auto& file_tree = get_dict(bfile_tree);

    auto ptr = &file_tree;
    for (const auto& file : storage) {
        // padding files are only for v1
        if (file.is_padding_file())
            continue;

        for (const auto& component : file.path()) {
            if (auto it = ptr->find(component); it != ptr->end()) {
                ptr = &get_dict(it->second);
            }
            else {
                auto [it2, suc] = ptr->insert_or_assign(component.string(), bc::bvalue(bc::btype::dict));
                ptr = &get_dict(it2->second);
            }
        }

        auto file_info = bc::bvalue(bc::btype::dict);

        if (file.attributes()) {
            file_info["attr"] = file.attributes().value();
        }

        file_info["length"] = file.file_size();
        file_info["pieces root"] = file.pieces_root();

        if (file.is_symlink()) {
            file_info["symlink path"] = file.symlink_path().value();
        }
        ptr->insert_or_assign("", file_info);
        // reset ptr to root of file tree
        ptr = &file_tree;
    }

    info.insert_or_assign("file tree", std::move(bfile_tree));

    if (m.name().empty()) {
        info.insert_or_assign("name", storage.root_directory().filename().string());
    }
    else {
        info.insert_or_assign("name", m.name());
    }

    info.insert_or_assign("piece length", storage.piece_size());
    info.insert_or_assign("pieces", make_v1_pieces_string(storage));
    info.insert_or_assign("meta version", 2);

    // only add private fields if it is true to minimize size
    if (auto is_private = m.is_private(); is_private) {
        info.insert_or_assign("private", is_private);
    }

    return binfo;
}


bencode::bvalue make_bvalue_common(const metafile& m)
{
    // add first element of announce-url to announce for compatibility
    auto torrent = bc::bvalue(bc::btype::dict);
    auto& btorrent = get_dict(torrent);

    if (!m.trackers().empty()) {
        btorrent.insert_or_assign("announce", m.trackers()[0].url);
        btorrent.insert_or_assign("announce-url", m.trackers());
    }
    if (!m.comment().empty()) {
        btorrent.insert_or_assign("comment", m.comment());
    }
    if (!m.created_by().empty()) {
        btorrent.insert_or_assign("created by", m.created_by());
    }
    if (m.creation_date().count() != 0) {
        btorrent.insert_or_assign("creation date", m.creation_date().count());
    }
    if (!m.collections().empty()) {
        btorrent.insert_or_assign("collections", m.collections());
    }
    if (!m.http_seeds().empty()) {
        btorrent.insert_or_assign("httpseeds", m.http_seeds());
    }
    if (!m.similar_torrents().empty()) {
        btorrent.insert_or_assign("similar", m.similar_torrents());
    }
    if (!m.source().empty()) {
        btorrent.insert_or_assign("source", m.source());
    }
    if (!m.web_seeds().empty()) {
        btorrent.insert_or_assign("url-list", m.web_seeds());
    }
    if (!m.dht_nodes().empty()) {
        btorrent.insert_or_assign("nodes", m.dht_nodes());
    }
    return torrent;
}

bencode::bvalue make_bvalue_v1(const metafile& m)
{
    namespace bc = bencode;
    auto torrent = make_bvalue_common(m);
    auto& btorrent = get_dict(torrent);

    btorrent.insert_or_assign("info", make_bvalue_infodict_v1(m));
    return torrent;
}

bencode::bvalue make_bvalue_v2(const dottorrent::metafile& m)
{
    namespace bc = bencode;
    auto torrent = make_bvalue_common(m);
    auto& btorrent = get_dict(torrent);
    const auto& storage = m.storage();

    // info dict
    btorrent.insert_or_assign("info", make_bvalue_infodict_v2(m));

    // piece layers

    auto bpiece_layers = bc::bvalue(bc::btype::dict);
    auto& piece_layers = get_dict(bpiece_layers);

    std::size_t f_idx = 0;
    for (const auto& file: storage) {
        if (file.file_size() > storage.piece_size()) {
            piece_layers.insert_or_assign(
                    std::string(std::string_view(file.pieces_root())),
                    make_v2_piece_layers_string(file));
        }
        ++f_idx;
    }

    btorrent.insert_or_assign("piece layers", piece_layers);
    return torrent;
}

bencode::bvalue make_bvalue_hybrid(const metafile& m)
{
    namespace bc = bencode;
    auto torrent = make_bvalue_common(m);
    auto& btorrent = get_dict(torrent);
    const auto& storage = m.storage();

    // info dict
    btorrent.insert_or_assign("info", make_bvalue_infodict_hybrid(m));

    // piece layers

    auto bpiece_layers = bc::bvalue(bc::btype::dict);
    auto& piece_layers = get_dict(bpiece_layers);

    std::size_t f_idx = 0;
    for (const auto& file: storage) {
        if (file.file_size() > storage.piece_size()) {
            piece_layers.insert_or_assign(
                    std::string(std::string_view(file.pieces_root())),
                    make_v2_piece_layers_string(file));
        }
        ++f_idx;
    }

    btorrent.insert_or_assign("piece layers", piece_layers);
    return torrent;
}



} // namespace dottorrent