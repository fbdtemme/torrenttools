//
// Created by fbdtemme on 3/24/20.
//
#include "verify.hpp"
#include "indicator.hpp"
#include "ls_colors.hpp"
#include "tree_view.hpp"
#include "cli_helpers.hpp"

#include <fmt/format.h>

#include "create.hpp"
#include "progress.hpp"


void configure_verify_app(CLI::App* app, verify_app_options& options)
{
    CLI::callback_t protocol_parser = [&](const CLI::results_t& v) -> bool {
        options.protocol_version = protocol_transformer(v);
        return true;
    };
    CLI::callback_t metafile_transformer = [&](const CLI::results_t& v) -> bool {
        options.metafile = target_transformer(v);
        return true;
    };
    CLI::callback_t files_transformer = [&](const CLI::results_t& v) -> bool {
        options.files_root_directory = target_transformer(v);
        return true;
    };

    app->add_option("metafile", metafile_transformer,
               "Metafile path.")
       ->required()
       ->type_name("<path>");

    app->add_option("target", files_transformer,
               "Target filename or directory to verify pieces for.")
       ->required()
       ->type_name("<path>");

    app->add_option("-v,--protocol", protocol_parser,
               "Set the bittorrent protocol to use. Options are 1, 2 or hybrid. [default: 1]")
       ->type_name("<protocol>")
       ->default_val("v1");

    app->add_option("-t, --threads", options.threads,
               "Set the number of threads to use for hashing. [default: 2]")
       ->type_name("<n>")
       ->default_val(2);


//    app->callback([&](){ run_verify_app(options); });
}


void run_verify_app(const main_app_options& main_options, const verify_app_options& options)
{
    verify_metafile(options.metafile);

    auto m = dottorrent::load_metafile(options.metafile);

    auto& file_storage = m.storage();
    // point the file storage to the target directory
    file_storage.set_root_directory(options.files_root_directory);


    dottorrent::storage_verifier_options verifier_options {
            .protocol_version = options.protocol_version,
            .threads = options.threads,
    };

    // no explicit protocol version given
    if (verifier_options.protocol_version == dottorrent::protocol::none) {
        verifier_options.protocol_version = m.storage().protocol();
    }

    bool simple_progress = false;
#ifdef __unix__
    bool runs_in_tty = true;
    runs_in_tty = isatty(STDOUT_FILENO);

    if (!runs_in_tty) {
        simple_progress = true;
    }
#endif

    auto verifier = dottorrent::storage_verifier(file_storage, verifier_options);

    fmt::print(std::cout, "Verifying files...\n");

    if (simple_progress) {
        run_with_simple_progress(std::cout, verifier, m);
    } else {
        run_with_progress(std::cout, verifier, m);
    }

    tree_options tree_options { .show_file_size = false, .show_directory_size = false};
    auto terminal_size = termcontrol::get_terminal_size();
    auto verify_file_tree = format_verify_file_tree(
            m, verifier, "  ",
            terminal_size.cols,
            tree_options);

    fmt::print(std::cout, "\nFiles:\n");
    fmt::print(std::cout, verify_file_tree);
}


void print_verify_statistics(const dottorrent::metafile& m, std::chrono::system_clock::duration duration)
{
    auto& storage = m.storage();

    std::string average_hash_rate_str{};
    using fsecs = std::chrono::duration<double>;
    auto seconds = std::chrono::duration_cast<fsecs>(duration).count();

    if (seconds != 0) {
        average_hash_rate_str = tt::format_hash_rate(storage.total_file_size()/seconds);
    }
    else {
        average_hash_rate_str = "∞ B/s";
    }

    std::cout << '\n' << std::endl;
    std::cout << fmt::format("Verifying completed in: {}\n", tt::format_duration(duration));
    std::cout << fmt::format("Average hash rate:      {}\n", average_hash_rate_str);
}
