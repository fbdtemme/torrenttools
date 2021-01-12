#pragma once
#include <string_view>
#include <string>
#include <chrono>
#include <filesystem>

#include <dottorrent/metafile.hpp>
#include <dottorrent/storage_verifier.hpp>

#include <CLI/CLI.hpp>
#include "argument_parsers.hpp"

namespace fs = std::filesystem;

using namespace std::string_view_literals;

struct verify_app_options
{
    fs::path metafile;
    fs::path files_root_directory;
    std::uint8_t threads;
    dottorrent::protocol protocol_version;
};


void run_verify_app(verify_app_options& options);

void print_verify_statistics(const dottorrent::metafile& m, std::chrono::system_clock::duration duration);

void configure_verify_app(CLI::App* app, verify_app_options& options);