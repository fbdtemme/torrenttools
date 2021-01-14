//
// Created by fbdtemme on 3/24/20.
//
#include "verify.hpp"
#include "indicator.hpp"
#include "utils.hpp"
#include "tree_view.hpp"
#include <fmt/format.h>

#include <cliprogressbar/application.hpp>
#include <cliprogressbar/widgets/bar.hpp>
#include <cliprogressbar/widgets/label.hpp>
#include <cliprogressbar/progress_indicator.hpp>
#include <cliprogressbar/formatters.hpp>
#include <create.hpp>



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

    app->add_option("torrent", metafile_transformer,
               "Torrent file path.")
       ->required()
       ->type_name("<path>");

    app->add_option("files", files_transformer,
               "Path of files to verify hashes for.")
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


void run_verify_app(verify_app_options& options)
{
    if (!fs::exists(options.metafile)) {
        throw std::invalid_argument("file does not exist");
    }
    if (fs::is_directory(options.metafile)) {
        throw std::invalid_argument("target must be a file");
    }

    auto m = dottorrent::load_metafile(options.metafile);

    auto& file_storage = m.storage();
    // point the file storage to the target directory
    file_storage.set_root_directory(options.files_root_directory);


    dottorrent::storage_verifier_options verifier_options {
            .protocol_version = options.protocol_version
    };

    // no explicit protocol version given
    if (verifier_options.protocol_version == dottorrent::protocol::none) {
        verifier_options.protocol_version = m.storage().protocol();
    }

    auto verifier = dottorrent::storage_verifier(file_storage, verifier_options);
    run_with_progress(verifier, m);
}


void run_with_progress(dottorrent::storage_verifier& verifier, const dottorrent::metafile& m)
{
    using namespace std::chrono_literals;
    namespace dt = dottorrent;

    std::size_t current_file_index = 0;
    auto& storage = m.storage();

    cliprogress::application app;

    std::cout << "Verifying files... " << std::endl;

    auto indicator = make_indicator(storage, storage.at(current_file_index));
    indicator->start();
    app.start();

    auto start_time = std::chrono::system_clock::now();
    verifier.start();

    // v1 torrents count padding files as regular files in their progress counters
    // v2 and hybrid torrents do not take padding files into account in their progress counters.
    std::size_t total_file_size;
    if (verifier.protocol() == dt::protocol::v1) {
        total_file_size = storage.total_file_size();
    } else {
        total_file_size = storage.total_regular_file_size();
    }

    while (verifier.bytes_done() < total_file_size) {
        auto [index, file_bytes_verified] = verifier.current_file_progress();

        // Current file has been completed, update last entry for the previous file(s) and move to next one
        if (index != current_file_index) {
            for ( ; current_file_index < index; ) {
                auto complete_size = storage.at(current_file_index).file_size();
                indicator->set_value(complete_size);
                on_indicator_completion(indicator);
                indicator->stop();

                indicator = make_indicator(storage, storage.at(++current_file_index));
                indicator->start();
            }
        }
        indicator->set_value(file_bytes_verified);
        std::this_thread::sleep_for(100ms);
    }

    auto complete_progress = storage.at(current_file_index).file_size();
    indicator->set_value(complete_progress);
    on_indicator_completion(indicator);
    indicator->stop();
    app.request_stop();
    app.wait();

    verifier.wait();

    auto stop_time = std::chrono::system_clock::now();
    auto total_duration = stop_time - start_time;

    tc::format_to(std::cout, tc::ecma48::character_position_absolute);
    tc::format_to(std::cout, tc::ecma48::erase_in_line);
    tc::format_to(std::cout, tc::ecma48::cursor_up, 3);

    print_verify_statistics(m, total_duration);
    std::cout << "\nFiles:\n";

    tree_options tree_options { .show_file_size = false, .show_directory_size = false};
    auto terminal_size = termcontrol::get_terminal_size();
    format_verify_file_tree(
            std::ostreambuf_iterator<char>{std::cout},
            m, verifier, "  ",
            terminal_size.cols,
            tree_options);
}


void print_verify_statistics(const dottorrent::metafile& m, std::chrono::system_clock::duration duration)
{
    auto& storage = m.storage();

    std::string average_hash_rate_str{};
    using fsecs = std::chrono::duration<double>;
    auto seconds = std::chrono::duration_cast<fsecs>(duration).count();

    if (seconds != 0) {
        average_hash_rate_str = format_hash_rate(storage.total_file_size()/seconds);
    }
    else {
        average_hash_rate_str = "∞ B/s";
    }

    std::cout << '\n' << std::endl;
    std::cout << fmt::format("Verifying completed in: {}\n", format_duration(duration));
    std::cout << fmt::format("Average hash rate:      {}\n", average_hash_rate_str);
}
