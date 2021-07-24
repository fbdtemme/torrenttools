#include "app_data.hpp"

fs::path get_user_data_dir()
{
    fs::path home_dir;

#if defined(__linux__)
    char* r = std::getenv("HOME");
    if (r) {
        home_dir = fs::path(r)/".config/torrenttools";
    }
#elif defined(__APPLE__)
    char* r = std::getenv("HOME");
    if (r) {
        home_dir = fs::path(r) /"Library/Application Support/torrenttools";
    }
#elif defined(_WIN32)
    char* r = std::getenv("APPDATA");
    if (r) {
        home_dir = fs::path(r) / "torrenttools";
    }
#endif

    return home_dir;
}

std::vector<fs::path> get_app_data_search_path()
{
    static std::vector<fs::path> data_dirs {
            get_user_data_dir(),
            GLOBAL_DATA_DIR,
            BUILD_DATA_DIR,
    };
    return data_dirs;
};
