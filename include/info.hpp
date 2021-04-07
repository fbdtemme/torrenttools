#pragma once
#include <string_view>
#include <filesystem>
#include <string>
#include <chrono>

#include <dottorrent/metafile.hpp>
#include <CLI/CLI.hpp>

#include "common.hpp"

namespace fs = std::filesystem;

using namespace std::string_view_literals;

enum class format_type { text, json };

struct info_app_options
{
    fs::path metafile;
    bool raw;
    bool show_pieces;
    bool show_padding_files;

    std::optional<std::string> query;
};


void configure_info_app(CLI::App* app, info_app_options& options);

void run_info_app(const main_app_options& main_options, const info_app_options& options);


struct formatting_options
{
    std::string_view entry_format = "{:<19}: {}\n"sv;
    std::string_view entry_continuation_format = "                     {}\n"sv;
    std::string_view piece_size_format = "{} ({} bytes)"sv;
    std::string_view creation_date_format = "{:%Y-%m-%d %H:%M:%S} UTC"sv;
    bool use_color = true;
    bool show_padding_files = false;

//    std::string_view creation_date_format = "{:%Y-%m-%d %H:%M:%S} UTC ({} POSIX time)"sv;
};

auto format_multiline(std::string_view key, std::string_view value,
                      const formatting_options& options = {}
                     ) -> std::string;

void format_announces(std::ostream& os, const dottorrent::metafile& metafile);

void general_info(std::ostream& os,
        const dottorrent::metafile& metafile,
        const std::filesystem::path& metafile_path,
        const formatting_options& options);

void create_general_info(
        std::ostream& os,
        const dottorrent::metafile& metafile,
        const std::filesystem::path& metafile_path,
        dottorrent::protocol protocol_version,
        const formatting_options& options);

void create_raw_info(
        std::ostream& os,
        const std::filesystem::path& metafile_path,
        bool include_binary = false);


void run_query(const fs::path& target, std::string query, bool include_binary = false);