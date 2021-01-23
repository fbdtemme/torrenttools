#include <iostream>
#include <locale>
#include <charconv>

#include <fmt/format.h>
#include <fmt/color.h>
#include <CLI/CLI.hpp>

#include <termcontrol/termcontrol.hpp>

#include "info.hpp"
#include "create.hpp"
#include "config.hpp"
#include "verify.hpp"
#include "query.hpp"
#include "help_formatter.hpp"



// TODO: Implement peer scrape to check number of seeders/leachers for each announce
//       without loading in a torrent client.



const std::string main_description = PROJECT_NAME " v" PROJECT_VERSION R"(
Tools for inspecting, creating and modifying bittorrent metafiles.
)";


void list_available_checksums()
{
    for (auto h : dottorrent::hasher_supported_algorithms()) {
        std::cout << to_string(h) << '\n';
    }
}


int main(int argc, char** argv) {
    std::setlocale(LC_ALL, "");
    std::ios_base::sync_with_stdio(false);

#ifdef WIN32
    if (!termcontrol::enable_virtual_terminal_processing()) {
        std::cout << "Warning: Could not enable ANSI escape code processing!" << std::endl;
    }
#endif

    info_app_options info_options {};
    create_app_options create_options {};
    verify_app_options verify_options {};

    CLI::App app(main_description, PROJECT_NAME);
    app.formatter(std::make_shared<help_formatter>());

    auto info_app    = app.add_subcommand("info",   "General information about bittorrent metafiles.");
    auto create_app  = app.add_subcommand("create", "Create bittorrent metafiles.");
    auto verify_app  = app.add_subcommand("verify", "Verify local data against bittorrent metafiles.");
//    auto edit_app    = app.add_subcommand("edit",   "Edit a bittorrent metafile.");

    /// List available checkusm
    app.add_flag_callback(
            "--checksum-algorithms", std::function(&list_available_checksums),
            "List the supported hash functions for per file checksums.");

    configure_info_app(info_app, info_options);
    configure_create_app(create_app, create_options);
    configure_verify_app(verify_app, verify_options);
//    configure_verify_app(edit_app, edit_options);

    try {
        app.parse(argc, argv);

        if (app.got_subcommand(create_app)) {
            run_create_app(create_options);
        }
        else if (app.got_subcommand(info_app)) {
            run_info_app(info_options);
        }
        else if (app.got_subcommand(verify_app)) {
            run_verify_app(verify_options);
        }
    }
    catch (const CLI::CallForHelp &e) {
        std::cout << app.help() << std::endl;
    }
    catch (const CLI::ParseError &e) {
        std::cerr << fmt::format(fg(fmt::terminal_color::red), "{}", e.what()) << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << fmt::format(fg(fmt::terminal_color::red), "Error: {}", e.what()) << std::endl;
    }

    if (app.count_all() == 1)
        std::cout << app.help();

    return EXIT_SUCCESS;
}