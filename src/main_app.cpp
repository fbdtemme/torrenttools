#include <iostream>
#include "config.hpp"
#include "show.hpp"
#include "argument_parsers.hpp"
#include "help_formatter.hpp"

#include <dottorrent/hasher/backend_info.hpp>

#ifdef _WIN32
#define UNICODE
#define _UNICODE
#include <windows.h>
#include <termcontrol/detail/windows.hpp>
#endif

void setup_console()
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

void list_available_checksums()
{
    auto h = dottorrent::hasher_supported_algorithms();
    auto list = std::vector(h.begin(), h.end());
    std::sort(list.begin(), list.end());

    for (auto l : list) {
        std::cout << to_string(l) << '\n';
    }
}

void print_version()
{
    fmt::print("{} v{}\n", PROJECT_NAME, PROJECT_VERSION);
    fmt::print("\nCryptographic backends:\n");

    for (auto [lib_name, lib_version] : dt::cryptographic_backends() ) {
        fmt::print("  {:<15} : {}\n", lib_name, lib_version);
    }
    std::cout << std::endl;
}

void configure_main_app(CLI::App* app, main_app_options& options)
{
    CLI::callback_t tracker_db_transformer = [&](const CLI::results_t& v) -> bool {
        options.tracker_db = config_path_transformer(v, /*check_exists=*/true);
        return true;
    };

    CLI::callback_t config_transformer = [&](const CLI::results_t& v) -> bool {
        options.config = config_path_transformer(v, /*check_exists=*/true);
        return true;
    };

    /// List available checksums
    app->add_flag_callback(
            "--checksum-algorithms", std::function(&list_available_checksums),
            "List the supported hash functions for per file checksums.");

    app->add_flag_callback(
            "-V,--version", std::function(&print_version),
            "Show program version and exit.");

    app->add_option(
               "--config", config_transformer,
               "Path to custom location for the config.yml file.")
       ->expected(1)
       ->type_name("<path>");

    app->add_option(
               "--trackers-config", tracker_db_transformer,
               "Path to custom location for the trackers.json file.")
       ->expected(1)
       ->type_name("<path>");
}
