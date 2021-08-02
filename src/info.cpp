#include <string_view>

#include <fmt/format.h>
#include <fmt/chrono.h>

#include <filesystem>
#include <string>
#include <chrono>
#include <ranges>
#include <bencode/bview.hpp>
#include <bencode/events/encode_json_to.hpp>
#include "dottorrent/metafile.hpp"
#include "dottorrent/serialization/all.hpp"


#include "info.hpp"
#include "argument_parsers.hpp"
#include "formatters.hpp"
#include "tree_view.hpp"
#include "config.hpp"

#ifdef __unix__
#include <unistd.h>
#endif

#include "escape_binary_fields.hpp"
#include "cli_helpers.hpp"

namespace fs = std::filesystem;
namespace dt = dottorrent;
namespace bc = bencode;
namespace rng = std::ranges;
namespace tt = torrenttools;

using namespace std::string_view_literals;
using namespace std::chrono_literals;
using namespace bencode::literals;

constexpr std::string_view program_name = PROJECT_NAME;
constexpr std::string_view program_version_string = PROJECT_VERSION;


void run_info_app(const main_app_options& main_options, const info_app_options& options)
{
    verify_metafile(options.metafile);

    if (options.raw) {
        create_raw_info(std::cout, options.metafile, options.show_pieces);
        return;
    }

    auto m = dottorrent::load_metafile(options.metafile);
    auto protocol_version = m.storage().protocol();
    formatting_options fmt_options {.show_padding_files = options.show_padding_files};

#ifdef __unix__
    if (!isatty(STDOUT_FILENO)) {
        fmt_options.use_color = false;
    }
#endif

    create_general_info(std::cout, m, options.metafile, protocol_version, fmt_options);
}



void configure_info_app(CLI::App* app, info_app_options& options)
{
    CLI::callback_t metafile_parser = [&](const CLI::results_t& v) -> bool {
        options.metafile = metafile_target_transformer(v);
        return true;
    };

    app->add_option("target", metafile_parser, "Target bittorrent metafile.")
          ->type_name("<path>")
          ->required();

    auto* raw_option = app->add_flag("--raw", options.raw,
            "Print the metafile data formatted as JSON. Binary data is filtered out.")
            ->default_val(false);

    auto* show_pieces_option = app->add_flag("--show-pieces", options.show_pieces,
            "Print the metafile data formatted as JSON.\n"
            "Binary data is included as hexadecimal strings.")
       ->default_val(false);

    show_pieces_option->needs(raw_option);

    app->add_flag("--show-padding-files", options.show_padding_files,
            "Show padding files in the file tree.")
            ->default_val(false);
}


auto format_multiline(std::string_view key,
                      std::string_view value,
                      const formatting_options& options) -> std::string
{
    std::vector<std::string_view> strings;
    std::string::size_type pos = 0;
    std::string::size_type prev = 0;
    std::string result;
    auto out = std::back_inserter(result);

    pos = value.find('\n', prev);
    fmt::format_to(out, fmt::runtime(options.entry_format), key, value.substr(prev, pos-prev));
    prev = pos + 1;

    while ((pos = value.find('\n', prev)) != std::string::npos) {
        fmt::format_to(out, fmt::runtime(options.entry_continuation_format), value.substr(prev, pos - prev));
        prev = pos + 1;
    }
    return result;
}

std::string format_indented_list(
        const std::string& key,
        const std::vector<std::string>& values,
        const formatting_options& options)
{
    std::string result;
    auto out = std::back_inserter(result);

    if (values.empty()) {
        fmt::format_to(out, fmt::runtime(options.entry_format), key, "");
        return result;
    }

    fmt::format_to(out, fmt::runtime(options.entry_format), key, values.at(0));
    for (std::size_t i = 1; i < values.size(); ++i) {
        fmt::format_to(out, fmt::runtime(options.entry_continuation_format), values.at(i));
    }
    return result;
}


// Pass protocol_version explicitly since not yet hashed torrent cannot query the protocol from
// the file_storage.
void create_general_info(std::ostream& os,
        const dt::metafile& metafile,
        const fs::path& metafile_path,
        dt::protocol protocol_version,
        const formatting_options& options)
{
    const auto& m = metafile;
    auto out = std::ostreambuf_iterator<char>(os);

    const std::string piece_size = fmt::format(
            fmt::runtime(options.piece_size_format),
            tt::format_size(m.piece_size()),
            m.piece_size());

    std::string creation_date;

    if (m.creation_date() != 0s) {
        std::time_t timestamp = m.creation_date().count();
        std::tm* datetime = std::gmtime(&timestamp);
        creation_date = fmt::format(
                fmt::runtime(options.creation_date_format), *datetime, timestamp);
    }
    const auto& entry = options.entry_format;

    const std::string& source_path = metafile_path.string();
    std::string protocol_version_string = tt::format_protocol_version(protocol_version);

    const std::string& comment = format_multiline("Comment"sv,         m.comment(), options);

    static const auto general_template = (
            "Metafile:          {metafile_path}\n"
            "Protocol version:  {protocol_version}\n"
            "{infohash_string}"
            "Piece size:        {piece_size}\n"
            "Piece count:       {piece_count}\n"
            "Created by:        {created_by}\n"
            "Created on:        {creation_date}\n"
            "Private:           {private}\n"
            "Name:              {name}\n"
            "Source:            {source}\n"
            "Comment:           {comment}\n"
    );

    // Check if torrent file is hashed so we can return to infohash
    std::string info_hash_string {};
    if (auto protocol = metafile.storage().protocol(); protocol != dt::protocol::none) {
        if ((protocol & dt::protocol::hybrid) == dt::protocol::hybrid ) {
            auto infohash_v1 = dt::info_hash_v1(metafile).hex_string();
            auto infohash_v2 = dt::info_hash_v2(metafile).hex_string();

            info_hash_string = fmt::format(
                    "Infohash:            v1: {}\n"
                    "                   v2: {}\n", infohash_v1, infohash_v2);
        }
        // v2-only
        else if ((protocol & dt::protocol::v2) == dt::protocol::v2) {
            auto infohash_v2 = dt::info_hash_v2(metafile).hex_string();
            info_hash_string = fmt::format(
                    "Infohash:          {}\n", infohash_v2);
        }
        // v1-only
        else if ((protocol & dt::protocol::v1) == dt::protocol::v1) {
            auto infohash_v1 = dt::info_hash_v1(metafile).hex_string();
            info_hash_string = fmt::format(
                    "Infohash:          {}\n", infohash_v1);
        }
    }



    fmt::format_to(out, fmt::runtime(general_template),
            fmt::arg("metafile_path",    metafile_path.string()),
            fmt::arg("protocol_version", protocol_version_string),
            fmt::arg("infohash_string",  info_hash_string),
            fmt::arg("piece_size",       piece_size),
            fmt::arg("piece_count",      m.piece_count()),
            fmt::arg("private",          m.is_private()),
            fmt::arg("created_by",       m.created_by()),
            fmt::arg("creation_date",    creation_date),
            fmt::arg("name",             m.name()),
            fmt::arg("source",           m.source()),
            fmt::arg("comment",          m.comment())
    );

    std::vector<std::string> similar_torrents_infohashes{};
    for (const auto& k: m.similar_torrents()) {
        switch (k.version()) {
        case dt::protocol::v1: {
            similar_torrents_infohashes.push_back(k.v1().hex_string());
            break;
        }
        case dt::protocol::v2: {
            similar_torrents_infohashes.push_back(k.v2().hex_string());
            break;
        }
        case dt::protocol::hybrid: {
            similar_torrents_infohashes.push_back(k.v1().hex_string());
            similar_torrents_infohashes.push_back(k.v2().hex_string());
            break;
        }
        }
    }

    format_announces(os, metafile, options);

    std::vector<std::string> dht_nodes_strings;
    rng::transform(m.dht_nodes(), std::back_inserter(dht_nodes_strings), [](auto node){ return std::string(node);});
    std::vector<std::string> collections(m.collections().begin(), m.collections().end());
    std::vector<std::string> other_info_fields {};
    rng::transform(m.other_info_fields(), std::back_inserter(other_info_fields), [](const auto& p) { return p.first; });

    rng::copy(format_indented_list("DHT nodes:",          dht_nodes_strings, options), out);
    rng::copy(format_indented_list("Web seeds:",          m.web_seeds(), options), out);
    rng::copy(format_indented_list("HTTP seeds:",         m.http_seeds(), options), out);
    rng::copy(format_indented_list("Similar torrents:",   similar_torrents_infohashes, options), out);
    rng::copy(format_indented_list("Collections:",        collections, options), out);
    rng::copy(format_indented_list("Other info fields:",  other_info_fields, options), out);

    tree_options tree_fmt_options {
        .use_color = options.use_color,
        .list_padding_files = options.show_padding_files
    };

    std::string file_tree;
    if (m.storage().file_count() < 1000) {
        file_tree = format_file_tree(m, "  ", tree_fmt_options);
    } else {
        file_tree = "\nMetafile contains more than 1000 files: skipping file tree ...\n";
    }
    auto file_stats = format_file_stats(m, " ", options.show_padding_files);

    fmt::format_to(out, "\nFiles:\n{}\n{}\n", file_tree, file_stats);
}


void format_announces(std::ostream& os, const dottorrent::metafile& metafile, const formatting_options& options)
{
    std::ostreambuf_iterator out {os};
    constexpr auto tracker_tier_entry = "tier {}  - {}"sv;
    constexpr auto tracker_entry      = "        - {}"sv;

    const auto& announce_urls = metafile.trackers();

    if (announce_urls.empty()) {
        fmt::format_to(out, fmt::runtime(options.entry_format), "Announce-urls:", "");
        return;
    }

    std::size_t tier_index = 0;
    auto [tier_begin, tier_end] = announce_urls.get_tier(tier_index);
    auto line = fmt::format(tracker_tier_entry, tier_index+1, *tier_begin++);
    fmt::format_to(out, fmt::runtime(options.entry_format), "Announce-urls:", line);

    // Finish current tier
    for ( ; tier_begin != tier_end; ++tier_begin) {
        line = fmt::format(tracker_entry, *tier_begin);
        fmt::format_to(out, fmt::runtime(options.entry_continuation_format), line);
    }

    // Finish other tiers
    for (tier_index += 1; tier_index < announce_urls.tier_count(); ++tier_index) {
        auto [tier_begin, tier_end] = announce_urls.get_tier(tier_index);
        auto line = fmt::format(tracker_tier_entry, tier_index+1, *tier_begin++);
        fmt::format_to(out, fmt::runtime(options.entry_continuation_format), line);

        for ( ; tier_begin != tier_end; ++tier_begin) {
            line = fmt::format(tracker_entry, *tier_begin);
            fmt::format_to(out, fmt::runtime(options.entry_continuation_format), line);
        }
    }
}




/// Format the raw structure but replace strings with raw bytes by placeholder strings.
void create_raw_info(std::ostream& os, const fs::path& metafile_path, bool include_binary)
{
    auto ifs = std::ifstream(metafile_path);
    auto bv = bencode::decode_value(ifs);

    if (!include_binary) {
        bv = escape_binary_metafile_fields(bv);
    }
    else {
        bv = escape_binary_metafile_fields_hex(bv);
    }
    auto formatter = bc::events::encode_json_to{std::cout};
    bencode::connect(formatter, bv);
    std::cout << std::endl;
}
