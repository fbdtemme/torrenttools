#include <dottorrent/magnet_uri.hpp>

#include "argument_parsers.hpp"
#include "magnet.hpp"
#include "cli_helpers.hpp"

namespace dt = dottorrent;

void configure_magnet_app(CLI::App* app, magnet_app_options& options)
{
    CLI::callback_t protocol_parser = [&](const CLI::results_t& v) -> bool {
        options.protocol = protocol_transformer(v);
        return true;
    };

    app->add_option("metafile", options.metafile, "Target bittorrent metafile.")
       ->type_name("<path>")
       ->required();

    app->add_option("-v,--protocol", protocol_parser,
            "Include only the infohash of the specified protocol for\n"
            "hybrid metafiles. Options are: 1, 2, hybrid [Default: hybrid].\n"
            "This option is only used for hybrid metafiles.\n"
            "When hybrid is specified, hybrid magnet URI's will\n"
            "include both the v1 and v2 infohash.")
        ->type_name("<protocol>")
        ->default_str("hybrid");
}

void run_magnet_app(const main_app_options& main_options, const magnet_app_options& options)
{
    verify_metafile(options.metafile);
    auto m = dt::load_metafile(options.metafile);

    dt::magnet_uri uri = dt::make_magnet_uri(m, options.protocol);
    std::cout << dt::encode_magnet_uri(uri) << std::endl;
}
