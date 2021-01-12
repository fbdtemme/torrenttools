#pragma once
#include <string_view>
#include <filesystem>
#include <string>
#include <chrono>
#include <dottorrent/metafile.hpp>

#include <CLI/CLI.hpp>

namespace fs = std::filesystem;

using namespace std::string_view_literals;

struct query_app_options
{
    fs::path metafile;
    std::string query;
};

void run_query_app(query_app_options& options);

void configure_query_app(CLI::App* app, query_app_options& options);