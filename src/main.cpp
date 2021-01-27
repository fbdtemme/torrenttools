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
#include "show.hpp"
#include "edit.hpp"
#include "magnet.hpp"

#include "help_formatter.hpp"

#ifdef _WIN32
#define UNICODE
#define _UNICODE
#include <windows.h>
#endif

static void setup_console()
{
    std::setlocale(LC_ALL, ".UTF-8");
    std::ios_base::sync_with_stdio(false);

#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    if (!termcontrol::enable_virtual_terminal_processing()) {
        std::cout << "Warning: Could not enable ANSI escape code processing!" << std::endl;
    }
#endif
}



const std::string main_description = PROJECT_NAME " v" PROJECT_VERSION R"(
Tools for inspecting, creating and modifying bittorrent metafiles.
)";


void list_available_checksums()
{
    for (auto h : dottorrent::hasher_supported_algorithms()) {
        std::cout << to_string(h) << '\n';
    }
}

void print_version()
{
    std::cout << PROJECT_VERSION << std::endl;
}


int main(int argc, char** argv) {

    setup_console();

    info_app_options info_options {};
    create_app_options create_options {};
    verify_app_options verify_options {};
    show_app_options show_options {};
    edit_app_options edit_options {};
    magnet_app_options magnet_options {};

    CLI::App app(main_description, PROJECT_NAME);
    app.formatter(std::make_shared<help_formatter>());
    app.get_formatter()->column_width(35);

    auto info_app    = app.add_subcommand("info",   "General information about bittorrent metafiles.");
    auto create_app  = app.add_subcommand("create", "Create bittorrent metafiles.");
    auto verify_app  = app.add_subcommand("verify", "Verify local data against bittorrent metafiles.");
    auto show_app    = app.add_subcommand("show",   "Show specific fields of bittorrent metafiles.");
    auto edit_app    = app.add_subcommand("edit",   "Edit bittorrent metafiles.");
    auto magnet_app  = app.add_subcommand("magnet", "Generate a magnet URI for bittorrent metafiles.");

    /// List available checksums
    app.add_flag_callback(
            "--checksum-algorithms", std::function(&list_available_checksums),
            "List the supported hash functions for per file checksums.");

    app.add_flag_callback(
            "-V,--version", std::function(&print_version),
            "Show program version and exit.");


    configure_info_app(info_app, info_options);
    configure_create_app(create_app, create_options);
    configure_verify_app(verify_app, verify_options);
    configure_show_app(show_app, show_options);
    configure_edit_app(edit_app, edit_options);
    configure_magnet_app(magnet_app, magnet_options);

    try {
        app.parse(argc, argv);

        if (app.got_subcommand(create_app)) {
            run_create_app(create_options);
        }
        else if (app.got_subcommand(edit_app)) {
            run_edit_app(edit_options);
        }
        else if (app.got_subcommand(info_app)) {
            run_info_app(info_options);
        }
        else if (app.got_subcommand(verify_app)) {
            run_verify_app(verify_options);
        }
        else if (app.got_subcommand(show_app)) {
            run_show_app(app.get_subcommand(show_app), show_options);
        }
        else if (app.got_subcommand(magnet_app)) {
            run_magnet_app(magnet_options);
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