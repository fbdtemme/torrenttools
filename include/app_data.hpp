#pragma once

#include <filesystem>
#include <vector>

#include "config.hpp"

namespace fs = std::filesystem;

/// Get the user data dir at runtime
static fs::path get_user_data_dir()
{
    fs::path home_dir;

#if defined(__linux__) | defined(__APPLE__)
    char* r = std::getenv("HOME");
    if (r) {
        home_dir = fs::path(r) / ".local/torrenttools";
    }

#elif defined(_WIN32)
    char* r = std::getenv("USERPROFILE");
    if (r) {
        home_dir = fs::path(r) / "AppData/torrenttools";
    }
#endif

    return home_dir;
}

inline std::vector<fs::path> get_app_data_search_path()
{
    static std::vector<fs::path> data_dirs {
            get_user_data_dir(),
            GLOBAL_DATA_DIR,
            BUILD_DATA_DIR,
    };
    return data_dirs;
};
