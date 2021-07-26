#include <filesystem>
#include <ranges>

#include <CLI/CLI.hpp>
#include <dottorrent/metafile.hpp>

#include "argument_parsers.hpp"
#include "cli_helpers.hpp"
#include "common.hpp"
#include "edit.hpp"
#include "config_parser.hpp"
#include "exceptions.hpp"

namespace dt = dottorrent;
namespace fs = std::filesystem;
namespace bc = bencode;
namespace tt = torrenttools;
namespace rng = std::ranges;

void configure_edit_app(CLI::App* app, edit_app_options& options)
{
    CLI::callback_t metafile_parser = [&](const CLI::results_t& v) -> bool {
        options.metafile = metafile_target_transformer(v);
        return true;
    };
    CLI::callback_t list_mode_parser =[&](const CLI::results_t& v) -> bool {
        options.list_mode = parse_list_edit_mode("--list-mode", v);
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

    CLI::callback_t creation_date_parser = [&](const CLI::results_t& v) -> bool {
        if (v.size() == 1 && v[0].empty()) {
            options.creation_date = std::chrono::system_clock::time_point {};
        }
        else {
            options.creation_date = creation_date_transformer("--creation-date", v);
        }
        return true;
    };

    CLI::callback_t private_flag_parser = [&](const CLI::results_t& v) -> bool {
        options.is_private = parse_explicit_flag("--private", v);
        return true;
    };

    CLI::callback_t similar_parser = [&](const CLI::results_t& v) -> bool {
        options.similar_torrents = similar_transformer("--similar", v);
        return true;
    };

    app->add_option("target", metafile_parser, "Target bittorrent metafile.")
                   ->type_name("<path>")
                   ->required();

    const auto max_size = 1U << 20U;

    app->add_option("-o,--output", options.destination,
               "Set the filename and/or output directory of the edited file.\n"
               "Default [<name>.torrent].\n"
               "This will overwrite the existing file if the name is the same.\n"
               "Use a path with trailing slash to only set the output directory.")
       ->type_name("<path>")
       ->expected(1);

    app->add_option("-m,--list-mode", list_mode_parser,
            "How to modify options that accept multiple arguments.\n"
            "Options are: append, prepend, replace. [Default: replace]\n"
            "The first character of these options is valid as well.")
        ->type_name("<list-mode>")
        ->expected(1);

    app->add_option("-a,--announce", announce_parser,
               "Add one or multiple announces urls.\n"
               "Multiple trackers will be added in seperate tiers by default.\n"
               "Use square brackets to groups urls in a single tier:\n"
               "eg. \"url1 [url1 url2]\"")
       ->type_name("<url>...")
       ->expected(0, max_size);

    app->add_option("-g,--announce-group", announce_group_parser,
               "Add the announce-urls defined from an announce group specified in the configuration file.\n"
               "Multiple groups can be passed."
               " eg. \"--announce-group group1 group2\"")
       ->type_name("<name>...")
       ->expected(0, max_size);

    app->add_option("-w, --web-seed", options.web_seeds,
               "Add one or multiple HTTP/FTP urls as seeds (GetRight-style).")
       ->type_name("<url>...")
       ->expected(0, max_size);

    app->add_option("--http-seed", options.http_seeds,
                    "Add one or multiple HTTP urls as seeds (Hoffman-style).")
            ->type_name("<url>...")
            ->expected(0, max_size);

    app->add_option("-d, --dht-node", dht_node_parser,
               "Add one or multiple DHT nodes.")
       ->type_name("<host:port>...")
       ->expected(0, max_size);


    app->add_option("-c, --comment", options.comment,
            "Replace the comment.\n "
            "Set to an empty string to remove the field.")
        ->type_name("<string>")
        ->expected(0, 1);

    app->add_option("-p, --private", private_flag_parser,
               "Set the private flag to disable DHT and PEX.\n"
               "Pass off to disable the flag.")
       ->type_name("<[on|off]>")
       ->expected(0, 1);

    app->add_option("-s, --source", options.source,
               "Replace the source tag.\n"
               "Set to an empty string to remove the field.")
       ->type_name("<source>")
       ->expected(0, 1);

    app->add_option("-n, --name", options.name,
               "Replace the name of the torrent.\n"
               "This changes the filename for single file torrents\n"
               "or the root directory name for multi-file torrents.\n")
       ->type_name("<name>")
       ->expected(1);

    auto* no_creation_date_option = app->add_flag_callback("--no-creation-date",
            [&]() { options.set_creation_date = false; },
            "Do not include the creation date.");

    auto* creation_date_option = app->add_option("--creation-date", creation_date_parser,
            "Replace the creation date field.\n"
            "Input is expected as an ISO-8601 or POSIX timestamp.\n"
            "Example: \"2021-01-22T18:21:46+0100\"\n"
            "Set to an empty string to remove the field.")
        ->type_name("<ISO-8601|POSIX time>")
       ->expected(0, 1);

    no_creation_date_option->excludes(creation_date_option);

    auto* no_created_by_option = app->add_flag_callback("--no-created-by",
            [&]() { options.set_created_by = false; },
            "Do not include the name and version of this program.");

    auto* created_by_option = app->add_option("--created-by", options.created_by,
               "Replace the created-by field.\n"
               "Set to an empty string to remove the field.")
       ->type_name("<string>")
       ->expected(0, 1);

    app->add_flag_callback("--stdout", [&]() { options.write_to_stdout = true; },
            "Write the edited metafile to the standard output");

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

    no_created_by_option->excludes(created_by_option);

    app->add_option("--profile,-P", options.profile,
                    "Read options form a config profile.")
            ->type_name("<profile-name>")
            ->expected(1);
}

void postprocess_edit_app(const CLI::App* app, const main_app_options& main_options, edit_app_options& options)
{
    auto [config_ptr, tracker_db_ptr] = load_config_and_tracker_db(main_options);

    if (config_ptr == nullptr && options.profile.has_value()) {
        throw tt::profile_error("configuration is required because profile was passed, but no configuration was found");
    }

    if (options.profile.has_value() && config_ptr != nullptr) {
        merge_edit_profile(*config_ptr, *options.profile, app, options);
    }
}

void merge_edit_profile(const tt::config& cfg, std::string_view profile_name, const CLI::App* app, edit_app_options& options)
{
    tt::profile profile;

    try {
        profile = cfg.get_profile(profile_name);
    }
    catch (const std::out_of_range& err) {
        throw std::invalid_argument("profile name does not exist");
    }

    if (profile.command != "edit") {
        throw std::invalid_argument("profile is not for edit command");
    }

    const auto& profile_options = std::get<edit_app_options>(profile.options);

    // Replace all options that are not set from the commandline with the profile defaults.
    if (app->get_option("--announce")->empty()) {
        options.announce_list = profile_options.announce_list;
    }
    if (app->get_option("--announce-group")->empty()) {
        options.announce_group_list = profile_options.announce_group_list;
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
    if (app->get_option("--http-seed")->empty()) {
        options.http_seeds = profile_options.http_seeds;
    }
    if (app->get_option("--list-mode")->empty()) {
        options.list_mode = profile_options.list_mode;
    }
    if (app->get_option("--name")->empty()) {
        options.name = profile_options.name;
    }
    if (app->get_option("--output")->empty()) {
        options.destination = profile_options.destination;
    }
    if (app->get_option("--private")->empty()) {
        options.is_private = profile_options.is_private;
    }
    if (app->get_option("--no-created-by")->empty()) {
        options.set_created_by = profile_options.set_created_by;
    }
    if (app->get_option("--no-creation-date")->empty()) {
        options.set_creation_date = profile_options.set_creation_date;
    }
    if (app->get_option("--similar")->empty()) {
        options.similar_torrents = profile_options.similar_torrents;
    }
    if (app->get_option("--source")->empty()) {
        options.source = profile_options.source;
    }
    if (app->get_option("--web-seed")->empty()) {
        options.web_seeds = profile_options.web_seeds;
    }
}


void run_edit_app(const main_app_options& main_options, const edit_app_options& options)
{
    auto m = dt::load_metafile(options.metafile);

    std::ostream& os = options.write_to_stdout ? std::cerr : std::cout;

    update_announce_group(m, main_options, options);
    update_announces(m, main_options, options);
    update_http_seeds(m, options);
    update_web_seeds(m, options);
    update_dht_nodes(m, options);

    if (options.comment) {
        m.set_comment(*options.comment);
    }
    if (options.source) {
        m.set_source(*options.source);
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

    if (options.name) {
        m.set_name(*options.name);
    }

    update_similar_torrents(m, options);
    update_collections(m, options);

    fs::path destination_file = get_destination_path(m, options.destination);
    auto out = std::ostreambuf_iterator(os);

    if (!options.write_to_stdout) {
        dt::save_metafile(destination_file, m, m.storage().protocol());
        fmt::format_to(out, "Metafile written to:  {}\n", destination_file.string());
    } else {
        dt::write_metafile_to(std::cout, m, m.storage().protocol());
        fmt::format_to(out, "Metafile written to standard output.");
    }
}


void update_announces(dt::metafile& m, const main_app_options& main_options, const edit_app_options& options)
{
    if (!options.announce_list.has_value()) {
        return;
    }

    auto [config, tracker_db] = load_config_and_tracker_db(main_options);
    auto announce_list = *options.announce_list;

    switch(options.list_mode) {
    case tt::list_edit_mode::replace : {
        m.clear_trackers();
        set_trackers(m, announce_list, tracker_db, config);
        return;
    }
    case tt::list_edit_mode::append: {
        set_trackers(m, announce_list, tracker_db, config);
        return;
    }
    case tt::list_edit_mode::prepend: {
        auto announces = dt::as_nested_vector(m.trackers());
        rng::copy(announces, std::back_inserter(announce_list));
        m.clear_trackers();
        set_trackers(m, announce_list, tracker_db, config);
        return;
    }
    }
}

void update_announce_group(dt::metafile& m, const main_app_options& main_options, const edit_app_options& options)
{
    if (!options.announce_group_list.has_value()) {
        return;
    }

    auto [config, tracker_db] = load_config_and_tracker_db(main_options);
    const auto& announce_group_list = *options.announce_group_list;


    switch(options.list_mode) {
    case tt::list_edit_mode::replace : {
        m.clear_trackers();
        set_tracker_group(m, announce_group_list, tracker_db, config);
        return;
    }
    case tt::list_edit_mode::append: {
        auto announces = dt::as_nested_vector(m.trackers());

        for (const auto& group_name : announce_group_list) {
            const auto& group_announces = config->get_announce_group(group_name);
            rng::transform(group_announces, std::back_inserter(announces),
                          [](const auto& s) { return std::vector<std::string>{s}; });

            set_trackers(m, announces, tracker_db, config);
        }
        return;
    }
    case tt::list_edit_mode::prepend: {
        auto announces = std::vector<std::vector<std::string>> {};

        for (const auto& group_name : announce_group_list) {
            const auto& group_announces = config->get_announce_group(group_name);
            rng::transform(group_announces, std::back_inserter(announces),
                           [](const auto& s) { return std::vector<std::string>{s}; });
        }
        rng::copy(dt::as_nested_vector(m.trackers()), std::back_inserter(announces));
        m.clear_trackers();
        set_trackers(m, announces, tracker_db, config);
        return;
    }
    }
}

void update_web_seeds(dt::metafile& m, const edit_app_options& options)
{
    if (!options.web_seeds.has_value()) {
        return;
    }
    auto web_seeds = *options.web_seeds;
    const auto add_web_seeds = [] (dt::metafile& m, const std::vector<std::string> seeds) {
        for (const auto& url : seeds) { m.add_web_seed(url); }
    };

    switch(options.list_mode) {
    case tt::list_edit_mode::replace : {
        m.clear_web_seeds();
        add_web_seeds(m, web_seeds);
        return;
    }
    case tt::list_edit_mode::append: {
        add_web_seeds(m, web_seeds);
        return;
    }
    case tt::list_edit_mode::prepend: {
        auto existing = m.web_seeds();
        rng::copy(existing, std::back_inserter(web_seeds));
        m.clear_web_seeds();
        add_web_seeds(m, web_seeds);
        return;
    }
    }
}


void update_http_seeds(dt::metafile& m, const edit_app_options& options)
{
    if (!options.http_seeds.has_value()) {
        return;
    }
    auto http_seeds = *options.http_seeds;
    const auto add_http_seeds = [] (dt::metafile& m, const std::vector<std::string>& seeds) {
        for (const auto& url : seeds) { m.add_http_seed(url); }
    };

    switch(options.list_mode) {
    case tt::list_edit_mode::replace : {
        m.clear_http_seeds();
        add_http_seeds(m, http_seeds);
        return;
    }
    case tt::list_edit_mode::append: {
        add_http_seeds(m, http_seeds);
        return;
    }
    case tt::list_edit_mode::prepend: {
        auto existing = m.http_seeds();
        rng::copy(existing, std::back_inserter(http_seeds));
        m.clear_http_seeds();
        add_http_seeds(m, http_seeds);
        return;
    }
    }
}

void update_dht_nodes(dt::metafile& m, const edit_app_options& options)
{
    if (!options.dht_nodes.has_value()) {
        return;
    }
    auto dht_nodes = *options.dht_nodes;
    const auto add_dht_nodes = [] (dt::metafile& m, const std::vector<dt::dht_node>& nodes) {
        for (const auto& n : nodes) { m.add_dht_node(n); }
    };

    switch(options.list_mode) {
    case tt::list_edit_mode::replace : {
        m.clear_dht_nodes();
        add_dht_nodes(m, dht_nodes);
        return;
    }
    case tt::list_edit_mode::append: {
        add_dht_nodes(m, dht_nodes);
        return;
    }
    case tt::list_edit_mode::prepend: {
        auto existing = m.dht_nodes();
        rng::copy(existing, std::back_inserter(dht_nodes));
        m.clear_dht_nodes();
        add_dht_nodes(m, dht_nodes);
        return;
    }
    }
}

void update_similar_torrents(dt::metafile& m, const edit_app_options& options)
{
    if (!options.similar_torrents.has_value()) {
        return;
    }
    auto similar_torrents = *options.similar_torrents;
    const auto add_similar_torrent = [] (dt::metafile& m, const std::vector<dt::info_hash>& infohashes) {
        for (const auto& ih : infohashes) { m.add_similar_torrent(ih); }
    };

    switch(options.list_mode) {
    case tt::list_edit_mode::replace : {
        m.clear_similar_torrents();
        add_similar_torrent(m, similar_torrents);
        return;
    }
    case tt::list_edit_mode::append: {
        add_similar_torrent(m, similar_torrents);
        return;
    }
    case tt::list_edit_mode::prepend: {
        auto existing = m.similar_torrents();
        rng::copy(existing, std::back_inserter(similar_torrents));
        m.clear_similar_torrents();
        add_similar_torrent(m, similar_torrents);
        return;
    }
    }
}

void update_collections(dt::metafile& m, const edit_app_options& options)
{
    if (!options.collections.has_value()) {
        return;
    }
    auto collections = *options.collections;
    const auto add_collections = [] (dt::metafile& m, const std::vector<std::string>& collections) {
        for (const auto& c : collections) { m.add_collection(c); }
    };

    switch(options.list_mode) {
    case tt::list_edit_mode::replace : {
        m.clear_collections();
        add_collections(m, collections);
        return;
    }
    case tt::list_edit_mode::append: [[fallthrough]];
    case tt::list_edit_mode::prepend: {
        add_collections(m, collections);
        return;
    }
    }
}


