#pragma once

#include <filesystem>

namespace fs = std::filesystem;


inline void verify_metafile(const fs::path& metafile)
{
    if (!fs::exists(metafile))
        throw std::invalid_argument(
                fmt::format("Metafile not found: {}", metafile.string()));

    if (fs::is_directory(metafile))
        throw std::invalid_argument(
                fmt::format("Target is a directory, not a metafile: {}", metafile.string()));
}