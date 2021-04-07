#include <iostream>
#include <fmt/format.h>

#include "config.hpp"
#include "create.hpp"
#include "edit.hpp"
#include "info.hpp"
#include "magnet.hpp"
#include "pad.hpp"
#include "show.hpp"
#include "verify.hpp"
#include "help_formatter.hpp"
#include "main_app.hpp"

const std::string main_description = PROJECT_NAME " v" PROJECT_VERSION R"(
Tools for inspecting, creating and modifying bittorrent metafiles.
)";

int main(int argc, char** argv) {

    setup_console();

    main_app_options main_options {};
    info_app_options info_options {};
    create_app_options create_options {};
    verify_app_options verify_options {};
    show_app_options show_options {};
    edit_app_options edit_options {};
    magnet_app_options magnet_options {};
    pad_app_options pad_options {};

    CLI::App app(main_description, PROJECT_NAME);
    app.formatter(std::make_shared<help_formatter>());
    app.get_formatter()->column_width(35);
    app.require_subcommand(1);
    app.fallthrough(true);

    configure_main_app(&app, main_options);

    auto info_app    = app.add_subcommand("info",   "General information about BitTorrent metafiles.");
    auto create_app  = app.add_subcommand("create", "Create BitTorrent metafiles.");
    auto verify_app  = app.add_subcommand("verify", "Verify local data against BitTorrent metafiles.");
    auto show_app    = app.add_subcommand("show",   "Show specific fields of BitTorrent metafiles.");
    auto edit_app    = app.add_subcommand("edit",   "Edit BitTorrent metafiles.");
    auto magnet_app  = app.add_subcommand("magnet", "Generate a magnet URI for BitTorrent metafiles.");
    auto pad_app     = app.add_subcommand("pad",    "Generate padding files for a BitTorrent metafile.");


    configure_info_app(info_app, info_options);
    configure_create_app(create_app, create_options);
    configure_verify_app(verify_app, verify_options);
    configure_show_app(show_app, show_options);
    configure_edit_app(edit_app, edit_options);
    configure_magnet_app(magnet_app, magnet_options);
    configure_pad_app(pad_app, pad_options);

    try {
        app.parse(argc, argv);

        if (app.got_subcommand(create_app)) {
            run_create_app(main_options, create_options);
        }
        else if (app.got_subcommand(edit_app)) {
            run_edit_app(main_options, edit_options);
        }
        else if (app.got_subcommand(info_app)) {
            run_info_app(main_options, info_options);
        }
        else if (app.got_subcommand(verify_app)) {
            run_verify_app(main_options, verify_options);
        }
        else if (app.got_subcommand(show_app)) {
            run_show_app(app.get_subcommand(show_app), main_options, show_options);
        }
        else if (app.got_subcommand(magnet_app)) {
            run_magnet_app(main_options, magnet_options);
        }
        else if (app.got_subcommand(pad_app)) {
            run_pad_app(main_options, pad_options);
        }
    }
    catch (const CLI::CallForHelp &e) {
        std::cout << app.help() << std::endl;
    }
    catch (const CLI::ParseError &e) {
        std::cerr << fmt::format("{}", e.what()) << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << fmt::format("Error: {}", e.what()) << std::endl;
    }

    if (app.count_all() == 1)
        std::cout << app.help();

    return EXIT_SUCCESS;
}