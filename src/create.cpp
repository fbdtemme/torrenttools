
#include <algorithm>
#include <functional>
#include <vector>
#include <string>
#include <ranges>
#include <optional>
#include <iostream>

#if defined(TORRENTTOOLS_USE_TBB)
#include <execution>
#endif

#include <fmt/format.h>
#include <CLI/CLI.hpp>
#include <CLI/Error.hpp>

#include <dottorrent/literals.hpp>
#include <termcontrol/termcontrol.hpp>

#include "create.hpp"
#include "file_matcher.hpp"
#include "formatters.hpp"
#include "info.hpp"
#include "argument_parsers.hpp"
#include "tracker_database.hpp"
#include "config_parser.hpp"
#include "progress.hpp"
#include "common.hpp"
#include "exceptions.hpp"

#ifdef __linux__
#include <unistd.h>
#endif

namespace tt = torrenttools;
namespace rng = std::ranges;
namespace tc = termcontrol;

// TODO: Try to handle too many levels of symbolic links.

using namespace std::string_literals;
using namespace std::chrono_literals;

constexpr std::string_view program_name = PROJECT_NAME;
constexpr std::string_view program_version_string = PROJECT_VERSION_STRING;

// TODO: Write torrent to cout.
// TODO: Add dry run options which will show all data but quits before hashing.

void configure_create_app(CLI::App* app, create_app_options& options)
{
    CLI::callback_t protocol_parser = [&](const CLI::results_t& v) -> bool {
        options.protocol_version = protocol_transformer(v);
        return true;
    };
    CLI::callback_t size_parser = [&](const CLI::results_t& v) -> bool {
        options.piece_size = piece_size_transformer(v);
        return true;
    };
    CLI::callback_t announce_parser =[&](const CLI::results_t& v) -> bool {
        options.announce_list = announce_transformer(v);
        return true;
    };
    CLI::callback_t announce_group_parser =[&](const CLI::results_t& v) -> bool {
        options.announce_group_list = v;
        return true;
    };
    CLI::callback_t dht_node_parser =[&](const CLI::results_t& v) -> bool {
        options.dht_nodes = dht_node_transformer(v);
        return true;
    };
    CLI::callback_t web_seeds_transformer =[&](const CLI::results_t& v) -> bool {
        options.web_seeds = seed_transformer("--web-seeds", v, /*allow_ftp=*/true);
        return true;
    };

    CLI::callback_t http_seeds_transformer =[&](const CLI::results_t& v) -> bool {
        options.http_seeds = seed_transformer("--http-seeds", v, /*allow_ftp=*/false);
        return true;
    };

    CLI::callback_t checksum_parser =[&](const CLI::results_t& v) -> bool {
        options.checksums = checksum_transformer(v);
        return true;
    };
    CLI::callback_t target_parser = [&](const CLI::results_t v) -> bool {
        auto target = path_transformer(v);
        if (target == "-") {
            options.read_from_stdin = true;
            std::string line {};
            std::getline(std::cin, line);
            options.target = path_transformer(std::vector{line});
        } else {
            options.target = target;
        }
        return true;
    };

    CLI::callback_t destination_parser = [&](const CLI::results_t v) -> bool {
        auto destination = path_transformer(v, /*check_exists=*/false);
        if (destination == "-") {
            options.write_to_stdout = true;
            options.destination = std::nullopt;
        } else {
            options.destination = destination;
        }
        return true;
    };

    CLI::callback_t io_block_size_parser = [&](const CLI::results_t& v) -> bool {
        options.io_block_size = io_block_size_transformer(v);
        return true;
    };
    CLI::callback_t private_flag_parser = [&](const CLI::results_t& v) -> bool {
        options.is_private = parse_explicit_flag("--private", v);
        return true;
    };

    CLI::callback_t creation_date_parser = [&](const CLI::results_t& v) -> bool {
        options.creation_date = creation_date_transformer("--creation-date", v);
        return true;
    };

    CLI::callback_t similar_parser = [&](const CLI::results_t& v) -> bool {
        options.similar_torrents = similar_transformer("--similar", v);
        return true;
    };

    const auto max_size = 1U << 20U;

    app->add_option("target", target_parser, "Target filename or directory")
       ->required()
       ->type_name("<path>")
       ->expected(1);

    options.protocol_version = dt::protocol::v1;
    app->add_option("-v,--protocol", protocol_parser,
               "Set the bittorrent protocol to use.\n "
               "Options are 1, 2 or hybrid. [default: 1]",
               false)
       ->type_name("<protocol>")
       ->expected(1);

    app->add_option("-o,--output", destination_parser,
               "Set the filename and/or output directory of the created file.\n"
               "[default: <name>.torrent]\n "
               "Use a path with trailing slash to only set the output directory.")
       ->type_name("<path>")
       ->expected(1);

    app->add_option("-a,--announce", announce_parser,
               "Add one or multiple announces urls.\n"
               "Multiple trackers will be added in seperate tiers by default. \n"
               "Use square brackets to groups urls in a single tier:\n"
               " eg. \"--announce url1 [url1 url2]\"")
       ->type_name("<url>...")
       ->expected(0, max_size);

    app->add_option("-g,--announce-group", announce_group_parser,
               "Add the announce-urls defined from an announce group specified in the configuration file.\n"
               "Multiple groups can be passed."
               " eg. \"--announce-group group1 group2\"")
       ->type_name("<name>...")
       ->expected(0, max_size);

    app->add_option("--http-seed", http_seeds_transformer,
                    "Add one or multiple HTTP urls as seeds (Hoffman-style).")
            ->type_name("<url>...")
            ->expected(0, max_size);

    app->add_option("-w, --web-seed", web_seeds_transformer,
               "Add one or multiple HTTP/FTP urls as seeds. (GetRight-style)")
       ->type_name("<url>...")
       ->expected(0, max_size);

    app->add_option("-d, --dht-node", dht_node_parser,
               "Add one or multiple DHT nodes.")
       ->type_name("<host:port>...")
       ->expected(0, max_size);

    // Allow an empty comment to force the precense of the field in the torrent file
    app->add_option("-c, --comment", options.comment,
               "Add a comment.")
       ->type_name("<string>");

    app->add_option("-p, --private", private_flag_parser,
               "Set the private flag to disable DHT and PEX.")
       ->type_name("<[on|off]>")
       ->expected(0, 1);

    app->add_option("-l, --piece-size", size_parser,
               "Set the piece size.\n"
               "When no unit is specified block size will be either 2^<n> bytes,\n"
               "or <n> bytes if n is larger or equal to 16384.\n"
               "Piece size must be a power of two in range [16K, 64M].\n"
               "Leave empty or set to auto to determine by total file size. [default: auto]")
       ->type_name("<size[K|M]>")
       ->expected(1);

    app->add_option("-s, --source", options.source,
               "Add a source tag to facilitate cross-seeding.")
       ->type_name("<source>")
       ->expected(1);

    options.enable_cross_seeding = true;
    auto* no_enable_cross_seeding = app->add_flag_callback("--no-cross-seed",
            [&]() { options.enable_cross_seeding = false; },
            "Do not include a hash of the announce urls to facilitate cross-seeding.");

    app->add_option("--collection", options.collections,
            "Add a collection name to this metafile.\n"
            "Other metafiles in the same collection are expected\n"
            "to share files with this one.")
       ->type_name("<name>...")
       ->expected(0, max_size);

    app->add_option("--similar", similar_parser,
               "Add a similar torrent by infohash or metafile.\n"
               "The similar torrent is expected to share some files with this one")
       ->type_name("<infohash|metafile>...")
       ->expected(0, max_size);

    app->add_option("-n, --name", options.name,
               "Set the name of the torrent. "
               "This changes the filename for single file torrents \n"
               "or the root directory name for multi-file torrents.\n"
               "[default: <basename of target>]")
       ->type_name("<name>")
       ->expected(1);

    // Set default;

    options.threads = 2;
    app->add_option("-t, --threads", options.threads,
               "Set the number of threads to use for hashing pieces. [default: 2]")
       ->type_name("<n>")
       ->expected(1);

    app->add_option("--checksum", checksum_parser,
               "Include a per file checksum of given algorithm." )
       ->type_name("<algorithm>...")
       ->expected(0, max_size);

    options.set_creation_date = true;
    auto* no_creation_date_option = app->add_flag_callback("--no-creation-date",
            [&]() { options.set_creation_date = false; },
            "Do not include the creation date.");

    auto* creation_date_option = app->add_option("--creation-date", creation_date_parser,
                                            "Override the value of the creation date field as ISO-8601 time or POSIX time.\n"
                                            "eg.: \"2021-01-22T18:21:46+0100\"")
                                    ->type_name("<ISO-8601|POSIX time>");

    no_creation_date_option->excludes(creation_date_option);

    options.set_created_by = true;
    auto* no_created_by_option = app->add_flag_callback("--no-created-by",
            [&]() { options.set_created_by = false; },
            "Do not include the name and version of this program.");

    auto* created_by_option = app->add_option("--created-by", options.created_by,
                                         "Override the value of the created by field.")
                                 ->type_name("<string>");

    no_created_by_option->excludes(created_by_option);

    app->add_option("--include", options.include_patterns,
               "Only add files matching given regex to the metafile.")
       ->type_name("<regex>...")
       ->expected(0, max_size);

    app->add_option("--exclude", options.exclude_patterns,
               "Do not add files matching given regex to the metafile.")
       ->type_name("<regex>...")
       ->expected(0, max_size);

    app->add_flag_callback("--include-hidden",
            [&]() { options.include_hidden_files = true; },
            "Do not skip hidden files.");

    app->add_option("--io-block-size", io_block_size_parser,
               "The size of blocks read from storage.\n"
               "Must be larger or equal to the piece size.")
       ->type_name("<size[K|M]>")
       ->expected(1);

    app->add_option("--profile,-P", options.profile,
            "Read options form a config profile.")
        ->type_name("<profile-name>")
        ->expected(1);

    options.simple_progress = false;
    app->add_flag_callback("--simple-progress",
            [&]() { options.simple_progress = true; },
            "Use simple progress reporting.");
}





void configure_matcher(torrenttools::file_matcher& matcher, const create_app_options& options)
{
    for (const auto& pattern : options.include_patterns ) {
        matcher.include_pattern(pattern);
    }
    for (const auto& pattern : options.exclude_patterns ) {
        matcher.exclude_pattern(pattern);
    }
    matcher.include_hidden_files(options.include_hidden_files);
    matcher.compile();
}



/// Select files and add the to the metafile
void set_files_with_progress(dottorrent::metafile& m, const create_app_options& options, std::ostream& os)
{
    auto out = std::ostreambuf_iterator(os);
    dottorrent::file_storage& storage = m.storage();

    // scan files and m
    if (fs::is_directory(options.target)) {
        torrenttools::file_matcher matcher{};
        configure_matcher(matcher, options);

        matcher.set_search_root(options.target);
        matcher.start();

        while (matcher.is_running()) {
            fmt::format_to(out, "\rScanning target directory: {} files processed", matcher.files_processed());
            std::flush(os);
            std::this_thread::sleep_for(50ms);
        }
        // wait for the thread to close and results to become available
        matcher.wait();
        std::cout << std::endl;

        auto files = matcher.results();

        fmt::format_to(out, "Sorting file list...");
        std::flush(os);

        #if defined(TORRENTTOOLS_USE_TBB)
            std::sort(std::execution::par_unseq, files.begin(), files.end(),
                    [](const fs::path& lhs, const fs::path& rhs) {
                        return rng::lexicographical_compare(lhs.string(), rhs.string());
                    }
            );
        #else
            std::sort(files.begin(), files.end(),
                    [](const fs::path& lhs, const fs::path& rhs) {
                        return rng::lexicographical_compare(lhs.string(), rhs.string());
                    }
            );
        #endif

        fmt::format_to(out, "\rSorting file list... Done.\n");
        std::flush(os);

        storage.set_root_directory(options.target);
        // target was a directory so if the torrent contains only a single file we will
        // still serialize it as a multi-file torrent with the directory included.
        storage.set_file_mode(dt::file_mode::multi);

        fmt::format_to(out, "Adding files to metafile...");
        std::flush(os);

        storage.add_files(files.begin(), files.end());
        fmt::format_to(out, "\rAdding files to metafile... Done.\n");
        std::flush(os);
    }
    else {
        storage.set_root_directory(options.target.parent_path());
        storage.set_file_mode(dt::file_mode::single);
        storage.add_file(options.target);
    }
    m.set_name(options.target.filename().string());
}

void postprocess_create_app(const CLI::App* app, const main_app_options& main_options, create_app_options& options)
{
    auto [config_ptr, tracker_db_ptr] = load_config_and_tracker_db(main_options);

    if (config_ptr == nullptr && options.profile.has_value()) {
        throw tt::profile_error("no configuration was found, but --profile requested.");
    }

    if (options.profile.has_value() && config_ptr != nullptr) {
        merge_create_profile(*config_ptr, *options.profile, app, options);
    }
}

void run_create_app(const main_app_options& main_options, create_app_options& options)
{
    namespace dt = dottorrent;
    using namespace dottorrent::literals;

    std::ostream& os = options.write_to_stdout ? std::cerr : std::cout;

    // create a new metafile
    dt::metafile m{};

    // add files to the file_storage
    auto& file_storage = m.storage();

    set_files_with_progress(m, options, os);

    if (options.piece_size) {
        file_storage.set_piece_size(*options.piece_size);
    } else {
        dottorrent::choose_piece_size(file_storage);
    }

    // announces
    if (!options.announce_group_list.empty()) {
        set_tracker_group(m, options.announce_group_list, tt::load_tracker_database(), tt::load_config());
    } else {
        set_trackers(m, options.announce_list, tt::load_tracker_database(), tt::load_config());
    }

    // web seeds (GetRight-style)
    for (const auto& url : options.web_seeds) {
        m.add_web_seed(url);
    }

    // http seeds (hoffman-style)
    for (const auto& http_url : options.http_seeds) {
        m.add_http_seed(http_url);
    }

    // dht nodes
    for (const auto& [host, port] : options.dht_nodes) {
        m.add_dht_node(host, port);
    }

    // add other options
    if (options.comment) {
        m.set_comment(*options.comment);
    }
    // Make sure we set this after set_tracker to be abe to override or remove the default source tag
    // when the tracker is in the tracker database.
    if (options.source) {
        m.set_source(*options.source);
    }

    if (options.enable_cross_seeding) {
        m.enable_cross_seeding();
        m.other_info_fields().clear();
    }

    // override the tracker default if explicitly set
    if (options.is_private.has_value()) {
        m.set_private(*options.is_private);
    }
    // Set created by the override value or the default if requested
    if (options.set_created_by) {
        if (options.created_by.has_value())
            m.set_created_by(*options.created_by);
        else
            m.set_created_by(CREATED_BY_STRING);
    }

    // Name
    if (options.name) {
        m.set_name(*options.name);
    }

    // Set created date to the override value or the default if requested
    if (options.set_creation_date) {
        if (options.creation_date.has_value())
            m.set_creation_date(*options.creation_date);
        else
            m.set_creation_date(std::chrono::system_clock::now());
    }

    // BEP 38: similar .
    if (!options.similar_torrents.empty()) {
        for (const auto& ih : options.similar_torrents) {
            m.add_similar_torrent(ih);
        }
    }

    // BEP 38: collection.
    if (!options.collections.empty()) {
        for (const auto& s : options.collections) {
            m.add_collection(s);
        }
    }

    fs::path destination_file = get_destination_path(m, options.destination);

    formatting_options fmt_options = {};

    bool simple_progress = options.simple_progress;

#ifdef __unix__
    bool runs_in_tty = true;
    if (options.write_to_stdout) {
        runs_in_tty = isatty(STDERR_FILENO);
    } else {
        runs_in_tty = isatty(STDOUT_FILENO);
    }
    if (!runs_in_tty) {
        fmt_options.use_color = false;
        simple_progress = true;
    }
#endif

    create_general_info(os, m, destination_file, options.protocol_version, fmt_options);
    os << '\n';

    if (options.io_block_size && *options.io_block_size < file_storage.piece_size()) {
        throw std::invalid_argument("io-block-size must be larger or equal to the piece size.");
    }

    // hash checking
    dt::storage_hasher_options hasher_options {
            .protocol_version = options.protocol_version,
            .checksums = {options.checksums},
            .min_io_block_size = options.io_block_size,
            .threads = options.threads
    };

    auto hasher = dt::storage_hasher(file_storage, hasher_options);

    os << "Hashing files..." << std::endl;

    if (simple_progress) {
        run_with_simple_progress(os, hasher, m);
    } else {
        run_with_progress(os, hasher, m);
    }

    // Join all threads and block until completed.
    if (!options.write_to_stdout) {
        dt::save_metafile(destination_file, m, options.protocol_version);
        os << fmt::format("Metafile written to: {}\n", destination_file.string());
    } else {
        os << fmt::format("Metafile written to standard output.");
        dt::write_metafile_to(std::cout, m, options.protocol_version);
    }
}


/// Merge options from the profile with options given on the commandline.
/// @returns options modified with defaults from the profile.
void merge_create_profile(const tt::config& cfg, std::string_view profile_name,
                          const CLI::App* app, create_app_options& options)
{
    tt::profile profile;

    try {
        profile = cfg.get_profile(profile_name);
    }
    catch (const std::out_of_range& err) {
        throw std::invalid_argument("profile name does not exist");
    }

    if (profile.command != "create") {
        throw std::invalid_argument("profile is not for create command");
    }

    const auto& profile_options = std::get<create_app_options>(profile.options);

    // Replace all options that are not set from the commandline with the profile defaults.
    if (app->get_option("--announce")->empty()) {
        options.announce_list = profile_options.announce_list;
    }
    if (app->get_option("--announce-group")->empty()) {
        options.announce_group_list = profile_options.announce_group_list;
    }
    if (app->get_option("--checksum")->empty()) {
        options.checksums = profile_options.checksums;
    }
    if (app->get_option("--collection")->empty()) {
        options.collections = profile_options.collections;
    }
    if (app->get_option("--comment")->empty()) {
        options.comment = profile_options.comment;
    }
    if (app->get_option("--created-by")->empty()) {
        options.created_by = profile_options.created_by;
    }
    if (app->get_option("--creation-date")->empty()) {
        options.creation_date = profile_options.creation_date;
    }
    if (app->get_option("--output")->empty()) {
        options.destination = profile_options.destination;
        options.write_to_stdout = profile_options.write_to_stdout;
    }
    if (app->get_option("--dht-node")->empty()) {
        options.dht_nodes = profile_options.dht_nodes;
    }
    if (app->get_option("--exclude")->empty()) {
        options.exclude_patterns = profile_options.exclude_patterns;
    }
    if (app->get_option("--http-seed")->empty()) {
        options.http_seeds = profile_options.http_seeds;
    }
    if (app->get_option("--include")->empty()) {
        options.include_patterns = profile_options.include_patterns;
    }
    if (app->get_option("--include-hidden")->empty()) {
        options.include_hidden_files = profile_options.include_hidden_files;
    }
    if (app->get_option("--io-block-size")->empty()) {
        options.io_block_size = profile_options.io_block_size;
    }
    if (app->get_option("--name")->empty()) {
        options.name = profile_options.name;
    }
    if (app->get_option("--output")->empty()) {
        options.destination = profile_options.destination;
    }
    if (app->get_option("--piece-size")->empty()) {
        options.piece_size = profile_options.piece_size;
    }
    if (app->get_option("--private")->empty()) {
        options.is_private = profile_options.is_private;
    }
    if (app->get_option("--protocol")->empty()) {
        options.protocol_version = profile_options.protocol_version;
    }
    if (app->get_option("--no-created-by")->empty()) {
        options.set_created_by = profile_options.set_created_by;
    }
    if (app->get_option("--no-creation-date")->empty()) {
        options.set_creation_date = profile_options.set_creation_date;
    }
    if (app->get_option("--no-cross-seed")->empty()) {
        options.enable_cross_seeding = profile_options.enable_cross_seeding;
    }
    if (app->get_option("--similar")->empty()) {
        options.similar_torrents = profile_options.similar_torrents;
    }
    if (app->get_option("--source")->empty()) {
        options.source = profile_options.source;
    }
    if (app->get_option("--threads")->empty()) {
        options.threads = profile_options.threads;
    }
    if (app->get_option("--web-seed")->empty()) {
        options.web_seeds = profile_options.web_seeds;
    }
}

