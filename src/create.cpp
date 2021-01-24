
#include <algorithm>
#include <functional>
#include <charconv>

#include <fmt/format.h>
#include <fmt/chrono.h>

#include <CLI/CLI.hpp>
#include <CLI/Error.hpp>

#include "dottorrent/literals.hpp"

#include "create.hpp"
#include "file_matcher.hpp"
#include "utils.hpp"
#include "natural_sort.hpp"
#include "info.hpp"
#include "argument_parsers.hpp"
#include "indicator.hpp"
#include "tree_view.hpp"
#include "tracker_database.hpp"
#include "config_parser.hpp"


// TODO: Try to handle too many levels of symbolic links. Cmake

//#include "progress.hpp"

using namespace std::string_literals;
using namespace std::chrono_literals;

constexpr std::string_view program_name = PROJECT_NAME;
constexpr std::string_view program_version_string = PROJECT_VERSION;

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
    CLI::callback_t dht_node_parser =[&](const CLI::results_t& v) -> bool {
        options.dht_nodes = dht_node_transformer(v);
        return true;
    };
    CLI::callback_t checksum_parser =[&](const CLI::results_t& v) -> bool {
        options.checksums = checksum_transformer(v);
        return true;
    };
    CLI::callback_t target_parser = [&](const CLI::results_t v) -> bool {
        options.target = target_transformer(v);
        return true;
    };
    CLI::callback_t io_block_size_parser = [&](const CLI::results_t& v) -> bool {
        options.io_block_size = io_block_size_transformer(v);
        return true;
    };
    CLI::callback_t private_flag_parser = [&](const CLI::results_t& v) -> bool {
        auto flag = parse_commandline_bool("--private", v);
        if (flag.has_value()) {
            options.is_private = flag;
        }
        // set implicit private for flag like behavior.
        options.is_private = true;
        return true;
    };

    CLI::callback_t creation_date_parser = [&](const CLI::results_t& v) -> bool {
        options.creation_date = creation_date_transformer("--creation-date", v);
        return true;
    };

    const auto max_size = 1U << 20U;

    app->add_option("target", target_parser, "Target filename or directory")
            ->required()
            ->type_name("<path>")
            ->expected(1);

    app->add_option("-v,--protocol", protocol_parser,
           "Set the bittorrent protocol to use. Options are 1, 2 or hybrid. [default: 1]")
            ->type_name("<protocol>")
            ->expected(1);

    app->add_option("-o,--output", options.destination,
            "Set the filename and/or output directory of the created file. [default: <name>.torrent]\n"
            "Use a path with trailing slash to only set the output directory.")
            ->type_name("<path>")
            ->expected(1);

    app->add_flag_callback("--stdout", [&]() { options.write_to_stdout = true; },
            "Write the metafile to the standard output");

    app->add_option("-a,--announce", announce_parser,
                    "Add one or multiple announces urls.\n"
                    "Multiple trackers will be added in seperate tiers by default. \n"
                    "Use square brackets to groups urls in a single tier:\n"
                    " eg. \"--announce url1 [url1 url2]\"")
            ->type_name("<url>")
            ->expected(0, max_size);

    app->add_option("-w, --web-seed", options.web_seeds,
                    "Add one or multiple HTTP/FTP urls as seeds.")
            ->type_name("<url>")
            ->expected(0, max_size);

    app->add_option("-d, --dht-node", dht_node_parser,
            "Add one or multiple DHT nodes.")
            ->type_name("<host:port>")
            ->expected(0, max_size);

    // Allow an empty comment to force the precense of the field in the torrent file
    app->add_option("-c, --comment", options.comment,
                    "Add a comment.")
            ->type_name("<string>");

    // TODO: Allow private flag to be overridden for supported trackers that require private flag.
    //       but generate a warning.
//    app->add_flag_callback("-p, --private", [&](){ options.is_private = true; },
//            "Set the private flag to disable DHT and PEX.");
    app->add_option("-p, --private", private_flag_parser,
            "Set the private flag to disable DHT and PEX.")
            ->type_name("<[on|off]>")
            ->expected(0, 1);

    app->add_option("-l, --piece-size", size_parser,
                    "Set the piece size.\n"
                    "When no unit is specified block size will be either 2^<n> bytes,\n"
                    "or <n> bytes if n is larger or equal to 16384.\n"
                    "Piece size must be a power of two in range [16K, 64M].\n"
                    "Leave empty to determine by total file size. [default: auto]")
            ->type_name("<size[K|M]>")
            ->expected(1);

    app->add_option("-s, --source", options.source,
                    "Add a source tag to facilitate cross-seeding.")
            ->type_name("<source>")
            ->expected(1);

    app->add_option("-n, --name", options.name,
                    "Set the name of the torrent. "
                    "This changes the filename for single file torrents \n"
                    "or the root directory name for multi-file torrents.\n"
                    "[default: <basename of target>]")
            ->type_name("<name>")
            ->expected(1);

    app->add_option("-t, --threads", options.threads,
                    "Set the number of threads to use for hashing pieces. [default: 2]")
            ->type_name("<n>")
            ->expected(1)
            ->default_val(2);

    app->add_option("--checksum", checksum_parser,
            "Include a per file checksum of given algorithm." )
            ->type_name("<algorithm>")
            ->expected(0, max_size)
            ->default_str("");

    auto* no_creation_date_option = app->add_flag_callback("--no-creation-date",
            [&]() { options.set_creation_date = false; },
            "Do not include the creation date.");

    auto* creation_date_option = app->add_option("--creation-date", creation_date_parser,
            "Override the value of the creation date field as ISO-8601 time or POSIX time.\n"
            "Example: \"2021-01-22T18:21:46Z+0100\"")
            ->type_name("<ISO-8601|POSIX time>");

    no_creation_date_option->excludes(creation_date_option);

    auto* no_created_by_option = app->add_flag_callback("--no-created-by",
            [&]() { options.set_created_by = false; },
            "Do not include the name and version of this program.");

    auto* created_by_option = app->add_option("--created-by", options.created_by,
            "Override the value of the created by field.")
            ->type_name("<string>");

    no_created_by_option->excludes(created_by_option);

    app->add_option("--include", options.include_patterns,
        "Only add files matching given regex to the metafile.")
        ->type_name("<regex>")
         ->expected(0, max_size);

    app->add_option("--exclude", options.exclude_patterns,
        "Do not add files matching given regex to the metafile.")
        ->type_name("<regex>")
        ->expected(0, max_size);

    app->add_flag_callback("--include-hidden",
            [&]() { options.include_hidden_files = true; },
            "Do not skip hidden files.");

    app->add_option("--io-block-size", io_block_size_parser,
               "The size of blocks read from storage.\n"
               "[default: max(1 MiB, <piece-size>)].\n"
               "Must be larger or equal to the piece size.")
       ->type_name("<size[K|M]>")
       ->expected(1);
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


void run_with_progress(std::ostream& os, dottorrent::storage_hasher& hasher, const dottorrent::metafile& m)
{
    using namespace std::chrono_literals;

    std::size_t current_file_index = 0;
    auto& storage = m.storage();

    os << "Hashing files..." << std::endl;
    cliprogress::application app(os);


    // v1 torrents count padding files as regular files in their progress counters
    // v2 and hybrid torrents do not take padding files into account in their progress counters.
    std::size_t total_file_size;
    std::unique_ptr<cliprogress::progress_indicator> indicator;

    if (hasher.protocol() == dt::protocol::v1) {
        total_file_size = storage.total_file_size();
        indicator = make_indicator(storage, storage.at(current_file_index));
    } else {
        total_file_size = storage.total_regular_file_size();
        indicator = make_indicator_v2(storage, storage.at(current_file_index));
    }

    indicator->start();
    app.start();

    auto start_time = std::chrono::system_clock::now();
    hasher.start();

    std::size_t index = 0;

    while (hasher.bytes_done() < total_file_size) {
        auto [index, file_bytes_hashed] = hasher.current_file_progress();

        // Current file has been completed, update last entry for the previous file(s) and move to next one
        if (index != current_file_index && index < storage.file_count()) {
            for ( ; current_file_index < index; ) {
                // set to 100%
                if (indicator) {
                    auto complete_size = storage.at(current_file_index).file_size();
                    indicator->set_value(complete_size);
                    on_indicator_completion(indicator);
                    indicator->stop();
                }

                ++current_file_index;

                if (hasher.protocol() == dt::protocol::v1) {
                    indicator = make_indicator(storage, storage.at(current_file_index));
                } else {
                    indicator = make_indicator_v2(storage, storage.at(current_file_index));
                }
                if (indicator) { indicator->start(); }
            }
        }
        if (indicator) { indicator->set_value(file_bytes_hashed); }
        std::this_thread::sleep_for(80ms);
    }

    if (indicator) {
        auto complete_progress = storage.at(current_file_index).file_size();
        indicator->set_value(complete_progress);
        on_indicator_completion(indicator);
        indicator->stop();
    }
    app.request_stop();
    app.wait();
    hasher.wait();

    tc::format_to(os, tc::ecma48::character_position_absolute);
    tc::format_to(os, tc::ecma48::erase_in_line);
    tc::format_to(os, tc::ecma48::cursor_up, 2);

    auto stop_time = std::chrono::system_clock::now();
    auto total_duration = stop_time - start_time;

    print_creation_statistics(os, m, total_duration);
}

void print_creation_statistics(std::ostream& os, const dottorrent::metafile& m, std::chrono::system_clock::duration duration)
{
    auto& storage = m.storage();

    std::string average_hash_rate_str {};
    using fsecs = std::chrono::duration<double>;
    auto seconds = std::chrono::duration_cast<fsecs>(duration).count();

    if (seconds != 0) {
        average_hash_rate_str = format_hash_rate(storage.total_file_size() / seconds);
    } else {
        average_hash_rate_str = "∞ B/s";
    }

    fmt::print(os, "Hashing completed in: {}\n", format_duration(duration));
    fmt::print(os, "Average hash rate:    {}\n",    average_hash_rate_str);

    // Torrent file is hashed so we can return to infohash
    std::string info_hash_string {};
    if (auto protocol = m.storage().protocol(); protocol != dt::protocol::none) {
        if ((protocol & dt::protocol::hybrid) == dt::protocol::hybrid ) {
            auto infohash_v1 = dt::info_hash_v1(m).hex_string();
            auto infohash_v2 = dt::info_hash_v2(m).hex_string();
            info_hash_string = fmt::format("Infohash:             v1: {}\n"
                                           "                      v2: {}\n", infohash_v1, infohash_v2);
        }
            // v2-only
        else if ((protocol & dt::protocol::v2) == dt::protocol::v2) {
            auto infohash_v2 = dt::info_hash_v2(m).hex_string();
            info_hash_string = fmt::format("Infohash:             {}\n", infohash_v2);
        }
            // v1-only
        else if ((protocol & dt::protocol::v1) == dt::protocol::v1) {
            auto infohash_v1 = dt::info_hash_v1(m).hex_string();
            info_hash_string = fmt::format("Infohash:             {}\n", infohash_v1);
        }
    }
    os << info_hash_string;
}



/// Select files and add the to the metafile
void set_files(dottorrent::metafile& m, const create_app_options& options)
{
    dottorrent::file_storage& storage = m.storage();

    // todo: filter out hidden files

    // scan files and m
    if (fs::is_directory(options.target)) {
        torrenttools::file_matcher matcher{};
        configure_matcher(matcher, options);
        auto files = matcher.run(options.target);
        std::sort(files.begin(), files.end(),
                [](const fs::path& lhs, const fs::path& rhs) {
                    return rng::lexicographical_compare(lhs.string(), rhs.string()); }
        );

        storage.set_root_directory(options.target);
        storage.add_files(files.begin(), files.end());
    }
    else {
        storage.set_root_directory(options.target.parent_path());
        storage.add_file(options.target);
    }
    m.set_name(options.target.filename().string());
}


void set_trackers(dottorrent::metafile& m, const create_app_options& options)
{
    const auto* config     = torrenttools::load_config();
    const auto* tracker_db = torrenttools::load_tracker_database();

    // a single tracker
    if (options.announce_list.size() == 1 && options.announce_list.at(0).size() == 1) {
        const auto& tracker = options.announce_list.at(0).at(0);

        if (tracker_db->contains(tracker)) {
            const auto& tracker_entry = tracker_db->at(tracker);
            m.add_tracker(tracker_entry.substitute_parameters(*config));
            m.set_private(tracker_entry.is_private);

            // set source tag to facilitate cross-seeding
            if (tracker_entry.is_private) {
                m.set_source(tracker_entry.name);
            }
        }
    }
    else {
        std::size_t tier_idx = 0;
        std::vector<bool> private_flags{};

        for (const auto& tier : options.announce_list) {
            for (const auto& tracker : tier) {

                // load data from tracker database
                if (bool in_db = tracker_db->contains(tracker)) {
                    const auto& tracker_entry = tracker_db->at(tracker);
                    m.add_tracker(tracker_entry.substitute_parameters(*config), tier_idx);
                    private_flags.push_back(tracker_entry.is_private);
                }

                m.add_tracker(tracker, tier_idx);
            }
            ++tier_idx;
        }

        if (rng::any_of(private_flags, std::identity{})) {
            m.set_private(true);
        }
    }
}

fs::path get_destination_path(dottorrent::metafile& m, const create_app_options& options)
{
    const auto* tracker_db = torrenttools::load_tracker_database();

    // complete destination: directory / filename
    fs::path destination {};
    // destination directory
    fs::path destination_directory {};
    // destination filename
    std::string destination_name {};

    if (options.destination) {
        // options is a complete path + filename
        if (options.destination->has_filename()) {
            destination = *options.destination;
            return destination;
        }
         // option is only a desination directory and not a filename
        else {
            destination_directory = *options.destination;
        }
    } else {
        destination_directory = fs::current_path();
    }

    // Single tracker torrent for which we know the tracker.
    if (m.trackers().size() == 1) {
        destination_name = fmt::format(
                "[{}]{}.torrent",
                tracker_db->at(m.trackers().front()).abbreviation,
                m.name());
    } else {
        destination_name = fmt::format("{}.torrent", m.name());
    }

    return destination_directory / destination_name;
};


void run_create_app(const create_app_options& options)
{
    namespace dt = dottorrent;
    using namespace dottorrent::literals;

    std::ostream& os = options.write_to_stdout ? std::cerr : std::cout;

    // create a new metafile
    dt::metafile m {};

    // add files to the file_storage
    auto& file_storage = m.storage();

    set_files(m, options);

    if (options.piece_size) {
        file_storage.set_piece_size(*options.piece_size);
    } else {
        dottorrent::choose_piece_size(file_storage);
    }

    // announces
    set_trackers(m, options);

    // web seeds
    for (const auto& url : options.web_seeds) {
        m.add_web_seed(url);
    }

    // dht nodes
    for (const auto& [host, port] : options.dht_nodes) {
        m.add_dht_node(host, port);
    }

    // add other options
    if (options.comment) {
        m.set_comment(*options.comment);
    }
    if (options.source) {
        m.set_comment(*options.source);
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

    // Set created date to the override value or the default if requested
    if (options.set_creation_date) {
        if (options.creation_date.has_value())
            m.set_creation_date(*options.creation_date);
        else
            m.set_creation_date(std::chrono::system_clock::now());
    }

    fs::path destination_file = get_destination_path(m, options);

    create_general_info(os, m, destination_file, options.protocol_version, {});
    os << '\n';

    std::size_t io_block_size = std::min(1_MiB, file_storage.piece_size());
    if (options.io_block_size) {
        io_block_size = *options.io_block_size;
    }

    // hash checking
    dt::storage_hasher_options hasher_options {
            .protocol_version = options.protocol_version,
            .checksums = {options.checksums},
            .min_chunk_size = io_block_size,
            .threads = options.threads
    };

    auto hasher = dt::storage_hasher(file_storage, hasher_options);
    run_with_progress(os, hasher, m);

    // Join all threads and block until completed.
    if (!options.write_to_stdout) {
        dt::save_metafile(destination_file, m, options.protocol_version);
        os << fmt::format("Metafile written to:  {}\n", destination_file.string());
    } else {
        os << fmt::format("Metafile written to standard output.");
        dt::write_metafile_to(std::cout, m, options.protocol_version);
    }
};
