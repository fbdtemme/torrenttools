#pragma once

#include <filesystem>
#include <dottorrent/general.hpp>

#include "common.hpp"

// Forward declarations
namespace CLI { class App; }

namespace dt = dottorrent;

struct show_app_options
{
    std::filesystem::path metafile;
    std::string subcommand;
    bool announce_flatten = false;
    bool piece_size_human_readable = false;
    bool file_size_human_readable = false;
    dt::protocol infohash_protocol = dt::protocol::hybrid;
    bool infohash_truncate = false;
    bool creation_date_iso_format = false;
    std::string query;
    bool query_show_binary;
    bool show_padding_files = false;
    std::filesystem::path files_prefix;
};


void configure_show_app(CLI::App* app, show_app_options& options);
void configure_show_common(CLI::App* subapp, show_app_options& options);

void configure_show_announce_subapp(CLI::App* announce_subapp, show_app_options& options);
void configure_show_comment_subapp(CLI::App* comment_subapp, show_app_options& options);
void configure_show_created_by_subapp(CLI::App* created_by_subapp, show_app_options& options);
void configure_show_creation_date_subapp(CLI::App* creation_date_subapp, show_app_options& options);
void configure_show_file_size_subapp(CLI::App* file_size_subapp, show_app_options& options);
void configure_show_files_subapp(CLI::App* files_subapp, show_app_options& options);
void configure_show_infohash_subapp(CLI::App* infohash_subapp, show_app_options& options);
void configure_show_name_subapp(CLI::App* name_subapp, show_app_options& options);
void configure_show_piece_size_subapp(CLI::App* piece_size_subapp, show_app_options& options);
void configure_show_private_subapp(CLI::App* private_subapp, show_app_options& options);
void configure_show_protocol_subapp(CLI::App* protocol_subapp, show_app_options& options);
void configure_show_query_subapp(CLI::App* source_subapp, show_app_options& options);
void configure_show_source_subapp(CLI::App* source_subapp, show_app_options& options);

void run_show_app(CLI::App* show_app, const main_app_options& main_options, const show_app_options& options);

void run_show_announce_subapp(const main_app_options& main_options, const show_app_options& options);
void run_show_comment_subapp(const main_app_options& main_options, const show_app_options& options);
void run_show_created_by_subapp(const main_app_options& main_options, const show_app_options& options);
void run_show_creation_date_subapp(const main_app_options& main_options, const show_app_options& options);
void run_show_file_size_subapp(const main_app_options& main_options, const show_app_options& options);
void run_show_files_subapp(const main_app_options& main_options, const show_app_options& options);
void run_show_infohash_subapp(const main_app_options& main_options, const show_app_options& options);
void run_show_name_subapp(const main_app_options& main_options, const show_app_options& options);
void run_show_piece_size_subapp(const main_app_options& main_options, const show_app_options& options);
void run_show_private_subapp(const main_app_options& main_options, const show_app_options& options);
void run_show_protocol_subapp(const main_app_options& main_options, const show_app_options& options);
void run_show_query_subapp(const main_app_options& main_options, const show_app_options& options);
void run_show_source_subapp(const main_app_options& main_options, const show_app_options& options);