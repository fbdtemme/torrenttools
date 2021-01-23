#include <algorithm>
#include <fstream>
#include <sstream>
#include <iostream>


#include <bencode/bencode.hpp>
#include <bencode/bvalue.hpp>
#include <bencode/events/encode_json_to.hpp>
#include <dottorrent/metafile_parsing.hpp>
#include <dottorrent/metafile_serialization.hpp>

#include "dottorrent/metafile.hpp"
#include "dottorrent/serialization/all.hpp"

namespace dottorrent {

using namespace std::string_view_literals;

const announce_url_list& metafile::trackers() const
{ return announce_list_; }

announce_url_list& metafile::trackers()
{ return announce_list_; }

void metafile::add_tracker(announce_url announce)
{
    auto& [url, tier] = announce;
    Expects(tier <= announce_list_.tier_count());

    // skip duplicate announces or update tier.
    if (auto it = announce_list_.find(url); it != announce_list_.end()) {
        if (it->tier != tier) {
            announce_url old = *it;
            announce_list_.insert(std::move(announce));
            announce_list_.erase(old);
        }
    }
    else {
        announce_list_.insert(std::move(announce));
    }
}

/// Add a announce url.
/// When no tier is specified each tracker is added in a seperate tier.
void metafile::add_tracker(std::string_view url, std::optional<std::size_t> tier)
{
    add_tracker(announce_url{url, tier ? *tier : announce_list_.tier_count()});
}

void metafile::remove_tracker(const announce_url& announce)
{ announce_list_.erase(announce); }

void metafile::remove_tracker(std::string_view url, std::optional<std::size_t> tier)
{
    if (!tier) {
        if (auto it = announce_list_.find(url); it != announce_list_.end()) {
            announce_list_.erase(it);
        }
    } else {
        remove_tracker(announce_url{url, *tier});
    }
}

void metafile::clear_trackers()
{ announce_list_.clear(); }

const std::vector<std::string>& metafile::http_seeds() const
{ return http_seeds_; }

std::vector<std::string>& metafile::http_seeds()
{ return http_seeds_; }

void metafile::add_http_seed(std::string_view url)
{
    if (auto it = std::find(http_seeds_.begin(), http_seeds_.end(), url);
            it == http_seeds_.end()) {
        http_seeds_.emplace_back(url);
    }
}

void metafile::remove_http_seed(std::string_view url)
{
    if (auto it = std::find(http_seeds_.begin(), http_seeds_.end(), url);
            it != http_seeds_.end()) {
        http_seeds_.erase(it);
    }
}

void metafile::clear_http_seeds()
{ http_seeds_.clear(); }

void metafile::add_web_seed(std::string_view url)
{
    if (auto it = std::find(web_seeds_.begin(), web_seeds_.end(), url);
            it == web_seeds_.end()) {
        web_seeds_.emplace_back(url);
    }
}

const std::vector<std::string>& metafile::web_seeds() const
{ return web_seeds_; }

void metafile::remove_web_seed(std::string_view url)
{
    if (auto it = std::find(web_seeds_.begin(), web_seeds_.end(), url);
            it != web_seeds_.end())
    {
        web_seeds_.erase(it);
    }
}

void metafile::clear_web_seeds()
{ web_seeds_.clear(); }

const std::vector<dht_node>& metafile::dht_nodes() const
{ return dht_nodes_; }

void metafile::add_dht_node(std::string_view url, uint16_t port)
{
    const auto entry = dht_node{std::string(url), port};

    if (auto it = std::find(dht_nodes_.begin(), dht_nodes_.end(), entry);
            it == dht_nodes_.end())
    {
        dht_nodes_.push_back(entry);
    }
}

void metafile::remove_dht_node(std::string_view url, uint16_t port)
{
    const auto entry = dht_node{std::string(url), port};

    if (auto it = std::find(dht_nodes_.begin(), dht_nodes_.end(), entry);
            it != dht_nodes_.end())
    {
        dht_nodes_.erase(it);
    }
}

void metafile::clear_dht_nodes()
{ dht_nodes_.clear(); }

const std::string& metafile::name() const
{ return name_; }

void metafile::set_name(std::string_view name)
{ name_ = name; }

const std::string& metafile::comment() const
{ return comment_; }

void metafile::set_comment(std::string_view comment)
{ comment_ = comment; }

const std::string& metafile::created_by() const
{ return created_by_; }

void metafile::set_created_by(std::string_view created_by)
{ created_by_ = created_by; }

std::chrono::seconds metafile::creation_date() const
{ return std::chrono::seconds(creation_date_); }

// TODO: [c++20] change system_clock to utc_clock for guaranteed unix epoch.
void metafile::set_creation_date(std::time_t time)
{ set_creation_date(std::chrono::system_clock::from_time_t(time)); }

const std::unordered_set<sha1_hash>& metafile::similar_torrents() const
{ return similar_torrents_; }

void metafile::add_similar_torrent(sha1_hash similar_torrent)
{ similar_torrents_.insert(similar_torrent); }

void metafile::remove_similar_torrent(sha1_hash similar_torrent)
{ similar_torrents_.erase(similar_torrent); }

void metafile::clear_similar_torrents()
{ similar_torrents_.clear(); }

const std::unordered_set<std::string>& metafile::collections() const
{ return collections_; }

void metafile::add_collection(std::string_view collection)
{ collections_.emplace(collection); }

// TODO: [C++20] switch to heterogenous key lookup
void metafile::remove_collection(std::string_view collection)
{ collections_.erase(std::string(collection)); }

void metafile::clear_collections()
{ collections_.clear(); }

const std::string& metafile::source() const
{ return source_; }

void metafile::set_source(std::string_view source)
{ source_ = source; }

bool metafile::is_private() const noexcept
{ return private_; }

void metafile::set_private(bool flag) noexcept
{ private_ = flag; }

const file_storage& metafile::storage() const noexcept
{ return storage_; }

file_storage& metafile::storage() noexcept
{ return storage_; }

std::size_t metafile::piece_size() const noexcept
{ return storage_.piece_size(); }

std::size_t metafile::piece_count() const noexcept
{ return storage_.piece_count(); }

std::size_t metafile::total_file_size() const noexcept
{ return storage_.total_file_size(); }

std::size_t metafile::total_regular_file_size() const noexcept
{ return storage_.total_regular_file_size(); }


metafile read_metafile(std::istream& is)
{
    bencode::bvalue data = bencode::decode_value(is);
    auto m = parse_metafile(data);
    return m;
}

metafile read_metafile(std::string_view view)
{
    namespace bc = bencode;
    auto descriptors = bencode::decode_view(view);
    auto data = descriptors.get_root();
    auto m = parse_metafile(data);
    return m;
}

metafile read_metafile(std::span<const std::byte> buffer)
{
    namespace bc = bencode;
    auto str_buffer = std::string_view(
            reinterpret_cast<const char*>(buffer.data()),
            buffer.size());

    auto descriptors = bencode::decode_view(str_buffer);
    auto data = descriptors.get_root();
    auto m = parse_metafile(data);
    return m;
}

std::string write_metafile(const metafile& m, protocol protocol_version)
{
    bencode::bvalue bv;
    if (protocol_version == protocol::v1) {
        bv = detail::make_bvalue_v1(m);
    }
    else if (protocol_version == protocol::v2) {
        bv = detail::make_bvalue_v2(m);
    }
    else if (protocol_version == (protocol::v2 | protocol::v1)) {
        bv = detail::make_bvalue_hybrid(m);
    } else {
        throw std::invalid_argument("unrecognised protocol version");
    }
    return bencode::encode(bv);
}


void write_metafile_to(std::ostream& os, const metafile& m, protocol protocol_version)
{
    bencode::bvalue bv;

    if (protocol_version == protocol::v1) {
        bv = detail::make_bvalue_v1(m);
    }
    else if (protocol_version == protocol::v2) {
        bv = detail::make_bvalue_v2(m);
    }
    else if (protocol_version == (protocol::v2 | protocol::v1)) {
        bv = detail::make_bvalue_hybrid(m);
    } else {
        throw std::invalid_argument("unrecognised protocol version");
    }
    bencode::encode_to(std::ostreambuf_iterator{os}, bv);
}


metafile load_metafile(const std::filesystem::path& path)
{
    std::ifstream ifs(path, std::ios::binary);
    auto buffer = std::string(
            std::istreambuf_iterator{ifs},
            std::istreambuf_iterator<char>{});
    auto m = read_metafile(buffer);
    return m;
}


void save_metafile(const std::filesystem::path& path, const metafile& m, protocol protocol_version)
{
    std::ofstream ofs(path, std::ios::binary);
    write_metafile_to(ofs, m, protocol_version);
}

/// Write a json representation of the metafile to `os`.
void serialize_json_to(std::ostream& os, const metafile& m, protocol protocol_version)
{
    bencode::events::encode_json_to to_json(std::cout);
    if (protocol_version == protocol::v1) {
        bc::connect(to_json, detail::make_bvalue_v1(m));
    }
    else if (protocol_version == protocol::v2) {
        bc::connect(to_json, detail::make_bvalue_v2(m));
    }
    else if (protocol_version == (protocol::v2 | protocol::v1)) {
        bc::connect(to_json, detail::make_bvalue_hybrid(m));
    } else {
        throw std::invalid_argument("unrecognised protocol version");
    }
}

std::string serialize_json(const metafile& metafile, protocol protocol_version)
{
    std::ostringstream os {};
    serialize_json_to(os, metafile, protocol_version);
    return os.str();
}

void dump_file(const metafile& m, const fs::path& dst)
{
    std::ofstream file(dst, std::ios::binary);
    write_metafile_to(file, m);
    file.close();
}

/// Returns the v1 info hash.
sha1_hash info_hash_v1(const metafile& m)
{
    auto s = bencode::encode(detail::make_bvalue_infodict_v1(m));
    sha1_hash hash {};
    auto hasher = make_hasher(hash_function::sha1);
    hasher->update({reinterpret_cast<const std::byte*>(s.data()), s.size()});
    hasher->finalize_to(hash);
    return hash;
}

/// Returns the v2 info hash
sha256_hash info_hash_v2(const metafile& m)
{
    std::string s;

    const auto protocol = m.storage().protocol();
    if (protocol == dottorrent::protocol::hybrid) {
        s = bencode::encode(detail::make_bvalue_infodict_hybrid(m));
    } else {
        s = bencode::encode(detail::make_bvalue_infodict_v2(m));
    }

    sha256_hash hash {};
    auto hasher = make_hasher(hash_function::sha256);
    hasher->update({reinterpret_cast<const std::byte*>(s.data()), s.size()});
    hasher->finalize_to(hash);
    return hash;
}


sha1_hash truncate_v2_hash(sha256_hash hash)
{
    return sha1_hash(std::span{hash.data(), sha1_hash::size_bytes});
}

} // namespace dottorrent