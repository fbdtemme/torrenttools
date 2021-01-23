#pragma once

#include <filesystem>
#include <vector>

#include "config.hpp"

namespace fs = std::filesystem;

/// Get the user data dir at runtime
fs::path get_user_data_dir();

/// Get the search path
std::vector<fs::path> get_app_data_search_path();
