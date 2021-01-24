#include <string_view>

#include <fmt/format.h>
#include <fmt/chrono.h>

#include <filesystem>
#include <string>
#include <chrono>
#include <bencode/bview.hpp>
#include <bencode/events/encode_json_to.hpp>
#include "dottorrent/metafile.hpp"
#include "dottorrent/serialization/all.hpp"
#include "info.hpp"
#include "utils.hpp"
#include "tree_view.hpp"
#include "config.hpp"

#ifdef __unix__
#include <unistd.h>
#endif

#include "escape_binary_fields.hpp"


namespace fs = std::filesystem;
namespace dt = dottorrent;
namespace bc = bencode;
namespace rng = std::ranges;

using namespace std::string_view_literals;
using namespace std::chrono_literals;
using namespace bencode::literals;

constexpr std::string_view program_name = PROJECT_NAME;
constexpr std::string_view program_version_string = PROJECT_VERSION;


void run_info_app(info_app_options& options)
{
    if (!fs::exists(options.metafile))
        throw std::invalid_argument(
                fmt::format("Metafile not found: {}", options.metafile.string()));

    if (fs::is_directory(options.metafile))
        throw std::invalid_argument(
                fmt::format("Target is a directory: {}", options.metafile.string()));

    if (options.raw) {
        create_raw_info(std::cout, options.metafile, options.show_pieces);
        return;
    }
    if (options.query) {
        run_query(options.metafile, *options.query);
        return;
    }

    auto m = dottorrent::load_metafile(options.metafile);
    auto protocol_version = m.storage().protocol();
    formatting_options fmt_options {.show_padding_files = options.show_padding_files};

#ifdef __unix__
    if (isatty(STDOUT_FILENO)) {
        fmt_options.use_color = true;
    } else {
        fmt_options.use_color = false;
    }
#endif

    create_general_info(std::cout, m, options.metafile, protocol_version, fmt_options);
}



void configure_info_app(CLI::App* app, info_app_options& options)
{
    app->add_option("target", options.metafile, "Target bittorrent metafile.")
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

    auto* query_options = app->add_option("--query", options.query,
            "Retrieve a field referenced by a bpointer in the target field.")
       ->type_name("<query>");

    query_options->excludes(raw_option);
    query_options->excludes(show_pieces_option);
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
    fmt::format_to(out, options.entry_format, key, value.substr(prev, pos-prev));
    prev = pos + 1;

    while ((pos = value.find('\n', prev)) != std::string::npos) {
        fmt::format_to(out, options.entry_continuation_format, value.substr(prev, pos - prev));
        prev = pos + 1;
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
            options.piece_size_format,
            format_size(m.piece_size()),
            m.piece_size());

    std::string creation_date;

    if (m.creation_date() != 0s) {
        std::time_t timestamp = m.creation_date().count();
        std::tm* datetime = std::gmtime(&timestamp);
        creation_date = fmt::format(
                options.creation_date_format, *datetime, timestamp);
    }
    const auto& entry = options.entry_format;

    const std::string& source_path = metafile_path.string();
    std::string protocol_version_string = format_protocol_version(protocol_version);

    const std::string& comment = format_multiline("Comment"sv,         m.comment(), options);

    static const auto general_template = (
            "Metafile:         {metafile_path}\n"
            "Protocol version: {protocol_version}\n"
            "{infohash_string}"
            "Piece size:       {piece_size}\n"
            "Created by:       {created_by}\n"
            "Created on:       {creation_date}\n"
            "Private:          {private}\n"
            "Name:             {name}\n"
            "Source:           {source}\n"
            "Comment:          {comment}\n\n"
    );

    // Torrent file is hashed so we can return to infohash
    std::string info_hash_string {};
    if (auto protocol = metafile.storage().protocol(); protocol != dt::protocol::none) {
        if ((protocol & dt::protocol::hybrid) == dt::protocol::hybrid ) {
            auto infohash_v1 = dt::info_hash_v1(metafile).hex_string();
            auto infohash_v2 = dt::info_hash_v2(metafile).hex_string();

            info_hash_string = fmt::format("Infohash:         v1: {}\n"
                                           "                  v2: {}\n", infohash_v1, infohash_v2);
        }
        // v2-only
        else if ((protocol & dt::protocol::v2) == dt::protocol::v2) {
            auto infohash_v2 = dt::info_hash_v2(metafile).hex_string();
            info_hash_string = fmt::format("Infohash:         {}\n", infohash_v2);
        }
        // v1-only
        else if ((protocol & dt::protocol::v1) == dt::protocol::v1) {
            auto infohash_v1 = dt::info_hash_v1(metafile).hex_string();
            info_hash_string = fmt::format("Infohash:         {}\n", infohash_v1);
        }
    }

    fmt::format_to(out, general_template,
            fmt::arg("metafile_path",    metafile_path.string()),
            fmt::arg("protocol_version", protocol_version_string),
            fmt::arg("infohash_string",  info_hash_string),
            fmt::arg("piece_size",       piece_size),
            fmt::arg("private",          m.is_private()),
            fmt::arg("created_by",       m.created_by()),
            fmt::arg("creation_date",    creation_date),
            fmt::arg("name",             m.name()),
            fmt::arg("source",           m.source()),
            fmt::arg("comment",          m.comment())
    );

    format_announces(os, metafile);

    tree_options tree_fmt_options {.use_color = options.use_color,
                                   .list_padding_files = options.show_padding_files};

    auto file_tree = format_file_tree(m, "  ", tree_fmt_options);
    auto file_stats = format_file_stats(m, " ", options.show_padding_files);

    fmt::format_to(out, "Files:\n{}\n{}\n", file_tree, file_stats);
}

void format_announces(std::ostream& os, const dottorrent::metafile& metafile)
{
    std::ostreambuf_iterator out {os};
    fmt::format_to(out, "Announces:\n");
    constexpr auto tracker_tier_entry = "tier {}  - {}\n"sv;
    constexpr auto tracker_entry      = "         - {}\n"sv;

    const auto& announce_urls = metafile.trackers();

    for (auto tier_index = 0; tier_index < announce_urls.tier_count(); ++tier_index) {
        auto [tier_begin, tier_end] = announce_urls.get_tier(tier_index);

        fmt::format_to(out, tracker_tier_entry, tier_index+1, *tier_begin++);
        for ( ; tier_begin != tier_end; ++tier_begin) {
            fmt::format_to(out, tracker_entry, *tier_begin);
        }
    }
    *out++ = '\n';
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

void run_query(const fs::path& target, std::string query, bool include_binary)
{
    std::ifstream ifs (target);
    std::string data(std::istreambuf_iterator<char>{ifs}, std::istreambuf_iterator<char>{});

    auto bv = bencode::decode_value(data);
    if (!include_binary) {
        bv = escape_binary_metafile_fields(bv);
    }
    else {
        bv = escape_binary_metafile_fields_hex(bv);
    }
    try {
        auto pointer = bc::bpointer(query);
        auto result = bv.at(pointer);
        bc::events::encode_json_to formatter(std::cout);
        bc::connect(formatter, result);
        std::cout << std::endl;
    }
    catch (const bc::bpointer_error& err) {
        auto help_message = fmt::format("Invalid query: {}", err.what());
        throw std::invalid_argument(help_message);
    }
    catch (const bc::out_of_range& err) {
        throw std::invalid_argument("No data matching query");
    }
}
