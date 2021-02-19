#pragma once
#include <string>
#include <vector>
#include <optional>
#include <cstdint>
#include <filesystem>
#include <chrono>
#include <dottorrent/metafile.hpp>

#include "config.hpp"

#include "dottorrent/storage_hasher.hpp"
#include "dottorrent/dht_node.hpp"
#include "info.hpp"


// forward declarations
namespace CLI { class App; }

namespace fs = std::filesystem;

struct pad_app_options
{
    std::filesystem::path metafile;
    std::filesystem::path target;
};

void configure_pad_app(CLI::App* app, pad_app_options& options);

void run_pad_app(const pad_app_options& options);