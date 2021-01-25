#include <filesystem>
#include <ranges>

#include <CLI/CLI.hpp>
#include <dottorrent/metafile.hpp>

#include "argument_parsers.hpp"
#include "cli_helpers.hpp"
#include "create.hpp"
#include "edit.hpp"

namespace dt = dottorrent;
namespace fs = std::filesystem;
namespace bc = bencode;
namespace tt = torrenttools;
namespace rng = std::ranges;

void configure_edit_app(CLI::App* app, edit_app_options& options)
{
    CLI::callback_t list_mode_parser =[&](const CLI::results_t& v) -> bool {
        options.list_mode = parse_list_edit_mode("--list-mode", v);
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

    app->add_option("target", options.metafile, "Target bittorrent metafile.")
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
       ->type_name("<url>... ")
       ->expected(0, max_size);

    app->add_option("-w, --web-seed", options.web_seeds,
               "Add one or multiple HTTP/FTP urls as seeds.")
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

    no_created_by_option->excludes(created_by_option);
}


void run_edit_app(const edit_app_options& options)
{
    verify_metafile(options.metafile);
    auto m = dt::load_metafile(options.metafile);

    std::ostream& os = options.write_to_stdout ? std::cerr : std::cout;

    update_announces(m, options);
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

    fs::path destination_file = get_destination_path(m, options.destination);

    if (!options.write_to_stdout) {
        dt::save_metafile(destination_file, m, m.storage().protocol());
        fmt::print(os, "Metafile written to:  {}\n", destination_file.string());
    } else {
        dt::write_metafile_to(std::cout, m, m.storage().protocol());
        fmt::print(os, "Metafile written to standard output.");
    }
}


void update_announces(dt::metafile& m, const edit_app_options& options)
{
    if (!options.announce_list.has_value()) {
        return;
    }
    auto announce_list = std::move(*options.announce_list);

    switch(options.list_mode) {
    case tt::list_edit_mode::replace : {
        m.clear_trackers();
        set_trackers(m, announce_list);
        return;
    }
    case tt::list_edit_mode::append: {
        set_trackers(m, announce_list);
        return;
    }
    case tt::list_edit_mode::prepend: {
        auto announces = dt::as_nested_vector(m.trackers());
        rng::copy(announces, std::back_inserter(announce_list));
        m.clear_trackers();
        set_trackers(m, announce_list);
        return;
    }
    }
}

void update_web_seeds(dt::metafile& m, const edit_app_options& options)
{
    if (!options.web_seeds.has_value()) {
        return;
    }
    auto web_seeds = std::move(*options.web_seeds);
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

void update_dht_nodes(dt::metafile& m, const edit_app_options& options)
{
    if (!options.dht_nodes.has_value()) {
        return;
    }
    auto dht_nodes = std::move(*options.dht_nodes);
    const auto add_dht_nodes = [] (dt::metafile& m, const std::vector<dt::dht_node> nodes) {
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
