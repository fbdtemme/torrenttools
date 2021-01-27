#pragma once

#include <string>
#include <vector>
#include <optional>
#include <cstdint>
#include <filesystem>
#include <chrono>

#include <CLI/App.hpp>
#include <dottorrent/general.hpp>

#include "config.hpp"

namespace fs = std::filesystem;
namespace dt = dottorrent;

struct magnet_app_options
{
    std::filesystem::path metafile;
    dt::protocol protocol = dt::protocol::hybrid;
};

void configure_magnet_app(CLI::App* app, magnet_app_options& options);

void run_magnet_app(const magnet_app_options& options);
