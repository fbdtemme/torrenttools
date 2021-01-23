#include <optional>
#include <concepts>
#include <queue>
#include <gsl-lite/gsl-lite.hpp>

#include "dottorrent/metafile_parsing.hpp"

namespace dottorrent::detail {

namespace bc = bencode;

using namespace std::string_view_literals;

template <bc::bvalue_or_bview T>
void parse_announce(const T& data, metafile& m)
{
    Expects(holds_dict(data));

    constexpr auto list_key = "announce-list"sv;
    constexpr auto key = "announce"sv;

    const auto& dict = get_dict(data);

    // parse_file announce-url
    try {
        if (auto it = dict.find(list_key); it != dict.end()) {
            std::size_t tier = 0;

            for (const auto& e : get_list(it->second)) {
                std::size_t item = 0;

                for (const auto& a : get_list(e)) {
                    const auto& s = get_string(a);
                    m.add_tracker(announce_url{s, tier});
                    ++item;
                }
                ++tier;
            }
        }
    } catch (const bencode::bad_access& e) {
        throw parse_error(list_key, e.what());
    }

    // parse_file announce if no announce-url field found
    try {
        auto& trackers = m.trackers();
        if (auto it = dict.find(key); it != dict.end()) {
            auto announce = get_string(it->second);

            if (const bool is_duplicate = trackers.contains(announce); !is_duplicate) {
                // verify that the first tier contains only this announce!
                if (trackers.tier_size(0) >= 1) {
                    auto tmp = trackers;
                    trackers.clear();
                    trackers.insert(announce_url(announce));
                    for (auto&& t : tmp) {
                        t.tier++;
                        trackers.insert(std::move(t));
                    }
                } else {
                    m.add_tracker(announce, 0);
                }
            }
        }
    } catch (const bencode::bad_access& e) {
        throw parse_error(key, e.what());
    }
}

template <bc::bvalue_or_bview T>
void parse_private(const T& data, metafile& m)
{
    Expects(holds_dict(data));
    const auto& dict = get_dict(data);
    Expects(dict.contains("info"));

    const auto& info_view = dict.at("info");
    const auto& info_dict = get_dict(info_view);
    constexpr auto key = "private"sv;

    try {
        if (const auto it = info_dict.find(key); it != info_dict.end()) {
            const auto v = get_integer(it->second);
            if (v > 1) {
                throw parse_error(key, fmt::format("expected integer equal to '0' or '1'"));
            }
            m.set_private(bool(v));
        }
    } catch (bencode::bad_access& e) {
        throw parse_error(key, e.what());
    }
}

template <bc::bvalue_or_bview T>
void parse_comment(const T& data, metafile& m)
{
    Expects(holds_dict(data));

    const auto& dict = get_dict(data);
    constexpr auto key = "comment"sv;

    try {
        if (const auto it = dict.find(key); it != dict.end()) {
            m.set_comment(get_string(it->second));
        }
    } catch (bc::bad_access& e) {
        throw parse_error(key, e.what());
    }
}

template <bc::bvalue_or_bview T>
void parse_created_by(const T& data, metafile& m)
{
    Expects(holds_dict(data));

    const auto& dict = get_dict(data);
    static constexpr auto key = "created by"sv;

    try {
        if (const auto it = dict.find(key); it != dict.end()) {
            m.set_created_by(get_string(it->second));
        }
    } catch (bencode::bad_access& e) {
        throw parse_error(key, e.what());
    }
}

template <bc::bvalue_or_bview T>
void parse_collections(const T& data, metafile& m)
{
    Expects(holds_dict(data));

    const auto& dict = bc::get_dict(data);
    constexpr auto key = "collections"sv;

    try {
        if (const auto it = dict.find(key); it != dict.end()) {
            const auto& desc = get_list(it->second);
            for (const auto& c : desc) {
                m.add_collection(get_string(c));
            }
        }
    } catch (bc::bad_access& e) {
        throw parse_error(key, e.what());
    }
}


template <bc::bvalue_or_bview T>
void parse_http_seeds(const T& data, metafile& m)
{
    Expects(holds_dict(data));

    const auto& dict = bc::get_dict(data);
    constexpr auto key = "httpseeds"sv;

    try {
        if (const auto it = dict.find(key); it != dict.end()) {
            const auto& desc = get_list(it->second);
            for (const auto& c : desc) {
                m.add_http_seed(get_string(c));
            }
        }
    } catch (bc::bad_access& e) {
        throw parse_error(key, e.what());
    }
}


template <bc::bvalue_or_bview T>
void parse_web_seeds(const T& data, metafile& m)
{
    Expects(holds_dict(data));

    const auto& dict = get_dict(data);
    constexpr auto key = "url-list"sv;

    try {
        if (const auto it = dict.find(key); it != dict.end()) {
            const auto& desc = get_list(it->second);
            for (const auto& c : desc) {
                m.add_web_seed(get_string(c));
            }
        }
    } catch (bc::bad_access& e) {
        throw parse_error(key, e.what());
    }
}

template <bc::bvalue_or_bview T>
void parse_dht_nodes(const T& data, metafile& m)
{
    Expects(holds_dict(data));

    const auto& dict = bc::get_dict(data);
    constexpr auto key = "dht"sv;

    try {
        if (const auto it = dict.find(key); it != dict.end()) {
            const auto desc = bc::get_list(it->second);
            for (const auto& c : desc) {
                const auto& tuple = bc::get_list(c);
                m.add_dht_node(
                        get_string(tuple[0]),
                        get_integer(tuple[1]));
            }
        }
    } catch (bencode::bad_access& e) {
        throw parse_error(key, e.what());
    }
}

template <bc::bvalue_or_bview T>
void parse_creation_date(const T& data, metafile& m)
{
    Expects(holds_dict(data));

    const auto& dict = bc::get_dict(data);
    constexpr auto key = "creation date"sv;

    try {
        if (const auto it = dict.find(key); it != dict.end()) {
            const auto desc = get_integer(it->second);
            m.set_creation_date(static_cast<std::time_t>(desc));
        }
    } catch (bencode::bad_access& e) {
        throw parse_error(key, e.what());
    }
}

template <bc::bvalue_or_bview T>
void parse_similar_torrents(const T& data, metafile& m)
{
    Expects(holds_dict(data));

    const auto& dict = get_dict(data);
    constexpr auto key = "similar"sv;

    try {
        if (const auto it = dict.find(key); it != dict.end()) {
            const auto& desc = get_list(it->second);
            for (const auto& c : desc) {
                m.add_similar_torrent(
                        make_hash_from_hex<sha1_hash>(get_string(c)));
            }
        }
    } catch (bc::bad_access& e) {
        throw parse_error(key, e.what());
    }
}

template <bc::bvalue_or_bview T>
void parse_name(const T& data, metafile& m)
{
    Expects(holds_dict(data));
    const auto& dict = bc::get_dict(data);
    Expects(dict.contains("info"));

    constexpr auto key = "name"sv;

    const auto& info_view = dict.at("info");
    const auto& info_dict = get_dict(info_view);

    try {
        if (const auto it = info_dict.find(key); it != info_dict.end()) {
            const auto& desc = get_string(it->second);
            m.set_name(desc);
        }
    }
    catch (bc::bad_access& e) {
        throw parse_error(key, e.what());
    }
}

template <bc::bvalue_or_bview T>
void parse_source(const T& data, metafile& m)
{
    Expects(holds_dict(data));
    const auto& dict = bc::get_dict(data);
    Expects(dict.contains("info"));

    constexpr auto key = "source"sv;

    const auto& info_view = dict.at("info");
    const auto& info_dict = get_dict(info_view);

    try {
        if (const auto it = info_dict.find(key); it != info_dict.end()) {
            const auto desc = get_string(it->second);
            m.set_source(desc);
        }
    }
    catch (bencode::bad_access & e) {
        throw parse_error(key, e.what());
    }
}


template <bc::bvalue_or_bview T>
fs::path parse_path(const T& data)
{
    Expects(holds_list(data));

    fs::path p{};
    auto& blist = get_list(data);
    for (auto& path_component : blist) {
        p /= bc::get_as<std::string_view>(path_component);
    }
    return p;
}

template <bc::bvalue_or_bview T>
std::optional<file_attributes> parse_file_attributes(const T& data)
{
    Expects(holds_dict(data));

    const auto& file_dict = get_dict(data);
    // file attributes
    std::optional<file_attributes> attributes = std::nullopt;

    if (auto it = file_dict.find("attr"); it != std::end(file_dict)) {
        const auto desc = get_string(it->second);
        attributes = make_file_attributes(desc);
    }

    return attributes;
}


template <bc::bvalue_or_bview T>
auto parse_file_entry_v1(const T& data) -> file_entry
{
    Expects(holds_dict(data));

    const auto& file_dict = get_dict(data);
    fs::path path {};

    // multi_file
    if (const auto it = file_dict.find("path"); it != file_dict.end()) {
        path = parse_path(it->second);
    }
    // single_file
    else {
        auto name_field_it = file_dict.find("name");
        if (name_field_it == file_dict.end()) {
            throw parse_error("name", "missing file name");
        }
        path = bencode::get_as<std::string_view>(name_field_it->second);
    }

    auto length_field_it = file_dict.find("length");
    if (length_field_it == file_dict.end()) {
        throw parse_error("name", "missing file size");
    }
    std::size_t size = bencode::get_integer(length_field_it->second);

    // file attributes
    std::optional<file_attributes> attributes = parse_file_attributes(data);

    // symlinks
    std::optional<fs::path> symlink = std::nullopt;

    if (auto it = file_dict.find("symlink"); it != file_dict.end()) {
        *symlink = parse_path(it->second);
    }

    file_entry entry(std::move(path), size, attributes, symlink);

    for (const auto& [k, v] : file_dict) {
        if (!is_hash_function_name(k))
            continue;

        const auto& s = get_string(v);
        auto checksum = make_checksum(
                k,
                std::span(reinterpret_cast<const std::byte*>(s.data()), s.size())
        );

        if (checksum != nullptr) {
            entry.add_checksum(std::move(checksum));
        }
    }
    return entry;
}


template <bc::bvalue_or_bview T>
static file_entry parse_file_entry_v2(const T& data, const fs::path& path)
{
    Expects(holds_dict(data));

    const auto& file_dict = get_dict(data);

    auto length_field_it = file_dict.find("length");
    if (length_field_it == file_dict.end()) {
        throw parse_error("name", "missing file size");
    }
    std::size_t size = get_integer(length_field_it->second);

    auto pieces_root_it = file_dict.find("pieces root");

    if (pieces_root_it == file_dict.end()) {
        throw parse_error("name", "missing pieces root");
    }
    auto pieces_root = sha256_hash(get_string(pieces_root_it->second));

    // file attributes
    std::optional<file_attributes> attributes = parse_file_attributes(data);

    // symlink
    std::optional<fs::path> symlink = std::nullopt;

    if (auto it = file_dict.find("symlink"); it != file_dict.end()) {
        *symlink = parse_path(it->second);
    }

    file_entry entry(path, size, attributes, symlink);
    entry.set_pieces_root(pieces_root);

    for (const auto& [k, v] : file_dict) {
        if (!is_hash_function_name(k))
            continue;

        const auto& s = get_string(v);
        auto checksum = make_checksum(
                k, std::span(reinterpret_cast<const std::byte*>(s.data()), s.size()));

        if (checksum != nullptr) {
            entry.add_checksum(std::move(checksum));
        }
    }
    return entry;
}


template <bc::bvalue_or_bview T>
void parse_file_list_v1(const T& data, metafile& m)
{
    Expects(holds_dict(data));
    const auto& dict = get_dict(data);
    Expects(dict.contains("info"));

    const auto& info_view = dict.at("info");
    const auto& info_dict = get_dict(info_view);

    auto& storage = m.storage();
    constexpr auto key = "files"sv;

    try {
        // multi file torrents
        if (const auto it = info_dict.find(key); it != info_dict.end()) {
            const auto& l = get_list(it->second);
            for (auto f : l) {
                storage.add_file(parse_file_entry_v1(f));
            }
        }
            // single file torrent
        else {
            storage.add_file(parse_file_entry_v1(dict.at("info")));
        }
    }
    catch (bencode::bad_access & e) {
        throw parse_error(key, e.what());
    }
}

template <bc::bvalue_or_bview T, std::output_iterator<file_entry> OutputIterator>
void parse_file_tree_v2(const T& data, metafile& m, OutputIterator out)
{
    Expects(holds_dict(data));
    const auto& dict = get_dict(data);
    Expects(dict.contains("info"));

    const auto& info_view = dict.at("info");
    const auto& info_dict = get_dict(info_view);

    auto& storage = m.storage();
    const auto& file_tree_view = info_dict.at("file tree");
    const auto& file_tree = get_dict(file_tree_view);

    using dict_type = typename T::dict_type;
    using iterator_type = typename T::dict_type::const_iterator;
    using frame_type = std::pair<std::reference_wrapper<const dict_type>, iterator_type>;

    // do not use recursion to be safe from stack overflow for very deep trees.
    std::stack<frame_type> frames {};
    // add a root to make frames and path_components equal sized.
    std::vector<std::string> path_components { "/" };

    frames.push(frame_type(file_tree, file_tree.begin()));

    while (!frames.empty()) {
        auto& frame = frames.top();
        const auto& dict = std::get<0>(frame).get();
        auto& it = std::get<1>(frame);

        if (it != rng::end(dict)) {
            const auto& key = it->first;
            const auto& value = bc::get_dict(it->second);

            // leaf node : a file
            if (key == "") {
                fs::path file_path{};
                for (const auto& component : path_components) {
                    file_path.append(component);
                }
                // file_entry paths are relative -> remove the root path componenet
                auto entry = parse_file_entry_v2(it->second, file_path.relative_path());
                *out++ = std::move(entry);
            }
            else {
                frames.push(frame_type(value, value.begin()));
                path_components.push_back(std::string(key));
            }
            ++it;
        } else {
            frames.pop();
            path_components.pop_back();
        }
    }
}

template <bc::bvalue_or_bview T>
void parse_file_tree_v2(const T& data, metafile& m)
{
    std::vector<file_entry> results {};
    parse_file_tree_v2(data, m, std::back_inserter(results));

    auto& storage = m.storage();
    for (auto&& r: results) {
        storage.add_file(std::move(r));
    }
}

template <bc::bvalue_or_bview T>
void parse_file_list_and_tree_hybrid(const T& data, metafile& m)
{
    // parse the v1 file list containing padding files.
    parse_file_list_v1(data, m);

    // add v2 data to the existing file_entries, validate v1 and v2 attributes
    auto& storage = m.storage();

    std::vector<file_entry> v2_entries {};
    parse_file_tree_v2(data, m, std::back_inserter(v2_entries));

    const auto path_comparator = [](const file_entry& f1, const file_entry& f2) {
        return f1.path() == f2.path();
    };

    for (auto& v2_entry : v2_entries) {
        auto it = std::find_if(storage.begin(), storage.end(), std::bind_front(path_comparator, v2_entry));
        it->set_pieces_root(v2_entry.pieces_root());
    }
};


template <bc::bvalue_or_bview T>
void parse_pieces_v1(const T& data, metafile& m)
{
    Expects(holds_dict(data));
    const auto& dict = get_dict(data);
    Expects(dict.contains("info"));

    const auto& info_view = dict.at("info");
    const auto& info_dict = get_dict(info_view);
    auto& storage = m.storage();

    if (auto it = info_dict.find("pieces"); it != info_dict.end()) {
        auto piece_string = get_string(it->second);
        auto piece_count = piece_string.size() / 20;

        storage.allocate_pieces();

        for (std::size_t idx = 0; idx < piece_count; ++idx) {
            auto p = sha1_hash(std::string_view(piece_string).substr(20*idx, 20));
            storage.set_piece_hash(idx, p);
        }
    } else {
        throw parse_error("info", "missing required field \"pieces\"");
    }
}

template <bc::bvalue_or_bview T>
void parse_piece_size(const T& data, metafile& m)
{
    Expects(holds_dict(data));
    const auto& dict = get_dict(data);
    Expects(dict.contains("info"));

    const auto& info_view = dict.at("info");
    const auto& info_dict = get_dict(info_view);
    auto& storage = m.storage();

    if (auto it = info_dict.find("piece length"); it!=info_dict.end()) {
        storage.set_piece_size(get_integer(it->second));
    }
    else {
        throw parse_error("info", "missing required field \"piece length\"");
    }
}

template <bc::bvalue_or_bview T>
void parse_piece_layers_v2(const T& data, metafile& m)
{
    Expects(holds_dict(data));
    const auto& dict = get_dict(data);
    constexpr auto key = "piece layers";
    auto& storage = m.storage();

    auto it = dict.find(key);
    if (it == dict.end()) {
        throw parse_error(key, "missing field");
    }
    if (!holds_dict(it->second))
        throw parse_error(key, "expected dict");

    const auto& layers_dict = get_dict(it->second);

    for (auto& f: storage) {
        // piece layers are only set for files larger then the piece size
        if (f.file_size() < m.storage().piece_size() || f.is_padding_file())
            continue;

        auto root = f.pieces_root();

        if (auto it2 = layers_dict.find(std::string_view(root)); it2 != layers_dict.end()) {
            std::vector<sha256_hash> layers;
            std::string_view layer_string = get_string(it2->second);

            for (auto idx = 0; idx < layer_string.size(); idx += sha256_hash::size_bytes) {
                layers.emplace_back(layer_string.substr(idx, sha256_hash::size_bytes));
            }
            f.set_piece_layer(std::span(layers));
        }
    }
}


template <bc::bvalue_or_bview T>
protocol parse_protocol(const T& data)
{
    Expects(holds_dict(data));
    const auto& dict = get_dict(data);
    Expects(dict.contains("info"));

    const auto& info_view = dict.at("info");
    const auto& info_dict = get_dict(info_view);

    constexpr auto key = "meta version"sv;

    try {
        if (auto it = info_dict.find(key); it != info_dict.end()) {
            auto meta_version = get_integer(it->second);
            if (meta_version == 1) {
                return protocol::v1;
            }
            if (meta_version == 2) {
                return protocol::v2;
            }
        }
        // no meta version field given : v1
        return protocol::v1;
    } catch (const bencode::bad_access& e) {
        throw parse_error(key, e.what());
    }
}

/// Check if a v2 torrent contains v1 compatibility data.
template <bc::bvalue_or_bview T>
bool check_if_hybrid(const T& data)
{
    Expects(holds_dict(data));
    const auto& dict = get_dict(data);
    Expects(dict.contains("info"));

    const auto& info_view = dict.at("info");
    const auto& info_dict = get_dict(info_view);

    if (info_dict.contains("pieces")) {
        return !get_string(info_dict.at("pieces")).empty();
    }
    return false;
}

template void parse_announce<>(const bc::bvalue& data, metafile& m);
template void parse_announce<>(const bc::bview& data, metafile& m);

template void parse_private<>(const bc::bvalue& data, metafile& m);
template void parse_private<>(const bc::bview& data, metafile& m);

template void parse_comment<>(const bc::bvalue& data, metafile& m);
template void parse_comment<>(const bc::bview& data, metafile& m);

template void parse_created_by<>(const bc::bvalue& data, metafile& m);
template void parse_created_by<>(const bc::bview& data, metafile& m);

template void parse_collections<>(const bc::bvalue& data, metafile& m);
template void parse_collections<>(const bc::bview& data, metafile& m);

template void parse_http_seeds<>(const bc::bvalue& data, metafile& m);
template void parse_http_seeds<>(const bc::bview& data, metafile& m);

template void parse_web_seeds<>(const bc::bvalue& data, metafile& m);
template void parse_web_seeds<>(const bc::bview& data, metafile& m);

template void parse_dht_nodes<>(const bc::bvalue& data, metafile& m);
template void parse_dht_nodes<>(const bc::bview& data, metafile& m);

template void parse_creation_date<>(const bc::bvalue& data, metafile& m);
template void parse_creation_date<>(const bc::bview& data, metafile& m);

template void parse_similar_torrents<>(const bc::bvalue& data, metafile& m);
template void parse_similar_torrents<>(const bc::bview& data, metafile& m);

template void parse_name<>(const bc::bvalue& data, metafile& m);
template void parse_name<>(const bc::bview& data, metafile& m);

template void parse_source<>(const bc::bvalue& data, metafile& m);
template void parse_source<>(const bc::bview& data, metafile& m);

template fs::path parse_path<>(const bc::bvalue& data);
template fs::path parse_path<>(const bc::bview& data);

template std::optional<file_attributes> parse_file_attributes<>(const bc::bvalue& data);
template std::optional<file_attributes> parse_file_attributes<>(const bc::bview& data);

template file_entry parse_file_entry_v1<>(const bc::bvalue& data);
template file_entry parse_file_entry_v1<>(const bc::bview& data);

template void parse_file_list_v1<>(const bc::bvalue& data, metafile& m);
template void parse_file_list_v1<>(const bc::bview& data, metafile& m);

template void parse_file_tree_v2<>(const bc::bvalue& data, metafile& m);
template void parse_file_tree_v2<>(const bc::bview& data, metafile& m);

template void parse_piece_size<>(const bc::bvalue& data, metafile& m);
template void parse_piece_size<>(const bc::bview& data, metafile& m);

template void parse_pieces_v1<>(const bc::bvalue& data, metafile& m);
template void parse_pieces_v1<>(const bc::bview& data, metafile& m);

template void parse_piece_layers_v2<>(const bc::bvalue& data, metafile& m);
template void parse_piece_layers_v2<>(const bc::bview& data, metafile& m);

template protocol parse_protocol<>(const bc::bvalue& data);
template protocol parse_protocol<>(const bc::bview& data);

template bool check_if_hybrid<>(const bc::bvalue& data);
template bool check_if_hybrid<>(const bc::bview& data);

} // namespace dottorrent::detail

namespace dottorrent {

namespace bc = bencode;

template<bc::bvalue_or_bview T>
metafile parse_metafile(const T& data)
{
    metafile m{};
    // parse the protocol header to determine if we have v1 or v2.
    auto protocol = detail::parse_protocol(data);

    // v1-only
    if (protocol == protocol::v1) {
        detail::parse_announce(data, m);
        detail::parse_private(data, m);
        detail::parse_comment(data, m);
        detail::parse_created_by(data, m);
        detail::parse_creation_date(data, m);
        detail::parse_collections(data, m);
        detail::parse_http_seeds(data, m);
        detail::parse_web_seeds(data, m);
        detail::parse_dht_nodes(data, m);
        detail::parse_similar_torrents(data, m);
        detail::parse_name(data, m);
        detail::parse_file_list_v1(data, m);
        detail::parse_piece_size(data, m);
        detail::parse_pieces_v1(data, m);
        detail::parse_source(data, m);
        return m;
    }
    // v2 or hybrid
    else if (protocol == protocol::v2) {
        detail::parse_announce(data, m);
        detail::parse_private(data, m);
        detail::parse_comment(data, m);
        detail::parse_created_by(data, m);
        detail::parse_creation_date(data, m);
        detail::parse_collections(data, m);
        detail::parse_http_seeds(data, m);
        detail::parse_web_seeds(data, m);
        detail::parse_dht_nodes(data, m);
        detail::parse_similar_torrents(data, m);
        detail::parse_name(data, m);
        detail::parse_piece_size(data, m);
        detail::parse_source(data, m);


        if (detail::check_if_hybrid(data)) {
            // we need the v1 file list to get info about padding files and set the correct total file size for v1.
            detail::parse_file_list_and_tree_hybrid(data, m);
            detail::parse_piece_layers_v2(data, m);
            detail::parse_pieces_v1(data, m);
        } else {
            detail::parse_file_tree_v2(data, m);
            detail::parse_piece_layers_v2(data, m);
        }

        return m;
    } else {
        throw parse_error("unsupported protocol version");
    }
}

template metafile parse_metafile<>(const bc::bvalue& data);
template metafile parse_metafile<>(const bc::bview& data);

}