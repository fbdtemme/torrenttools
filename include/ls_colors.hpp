#pragma once

#include <cstdlib>
#include <array>
#include <string>
#include <type_traits>
#include <string_view>
#include <cmath>
#include <charconv>
#include <map>

#include <filesystem>

#include <fmt/format.h>
#include <fmt/color.h>

#include <termcontrol/detail/fmt.hpp>
#include <dottorrent/general.hpp>
#include <dottorrent/file_entry.hpp>

namespace fs = std::filesystem;

namespace detail {

/// Parse the LS_COLORS string into a map of text_styles.
inline std::map<std::string, termcontrol::text_style>
parse_ls_colors(std::string_view ls_colors)
{
    using namespace std::string_literals;
    std::map<std::string, termcontrol::text_style> ls_style_map {};

    std::size_t prev = 0, pos = 0, idx = 0, sep_prev = 0, sep = 0;
    std::uint8_t emphasis;
    std::uint8_t foreground;
    std::uint8_t background;

    while ((pos = ls_colors.find(':', prev)) != std::string_view::npos) {
        auto entry = ls_colors.substr(prev, pos-prev);
        std::size_t equal_sign_pos = entry.find('=');
        auto key = entry.substr(0, equal_sign_pos);
        auto value = entry.substr(equal_sign_pos+1);

        emphasis = 0;
        foreground = static_cast<std::uint8_t>(termcontrol::terminal_color::black);
        background = 49;

        if (auto sep = value.find(';'); sep != std::string_view::npos) {
            auto style_str = value.substr(0, sep);

            if (auto[p, ec] = std::from_chars(style_str.begin(), style_str.end(), emphasis);
                    ec != std::errc())
            {
                continue;
            }
            sep_prev = sep + 1;

            if (sep = value.substr(sep_prev).find(';'); sep != std::string_view::npos) {
                sep_prev = sep + 1;
                if (sep = value.substr(sep_prev).find(';'); sep != std::string_view::npos) {
                    auto background_str = value.substr(sep_prev, sep-sep_prev);

                    if (auto[p, ec] = std::from_chars(background_str.begin(), background_str.end(), background);
                            ec != std::errc())
                    {
                        continue;
                    }
                }
            }
            else {
                auto foreground_str = value.substr(sep_prev, sep-sep_prev);

                if (auto[p, ec] = std::from_chars(foreground_str.begin(), foreground_str.end(), foreground);
                        ec != std::errc()) {
                    continue;
                }
            }
        }
        prev = pos + 1;

        termcontrol::text_style style = termcontrol::emphasis{emphasis} |
                fg(termcontrol::terminal_color(foreground)) |
                bg(termcontrol::terminal_color(background));
        ls_style_map.insert_or_assign(std::string(key), style);
    }

    return ls_style_map;
}

} // namespace detail

struct ls_colors
{
    using style_type = termcontrol::text_style;
    using style_map = std::map<std::string, style_type>;

    struct ls_colors_data
    {
        style_type normal;                 ///< Normal (nonfilename) text
        style_type file;                   ///< Regular file
        style_type dir;                    ///< Directory
        style_type link;                   ///< Symbolic link
        style_type exec;                   ///< Executable file
    };

    explicit ls_colors()
        : data_()
        , extensions_()
    {
       load_system_colors();
    }

    termcontrol::text_style file_style(const dottorrent::file_entry& entry) const
    {
        if (entry.is_symlink()) {
            return data_.link;
        }
        else if (entry.is_executable()) {
            return data_.exec;
        }
        else {
            if (entry.path().has_extension()) {
                const auto& ext = entry.path().extension().string();

                // Prefer an extension
                if (auto it = extensions_.find(ext); it != extensions_.end()) {
                    return it->second;
                }
            }

            return data_.normal;
        }
    }

    termcontrol::text_style directory_style() const
    {
        return data_.dir;
    }

private:
    bool load_system_colors() noexcept
    {
        const char* lc_colors_p = std::getenv("LS_COLORS");
        if (!lc_colors_p)
            return false;

        auto map = detail::parse_ls_colors(lc_colors_p);

        data_.normal                = map["no"];
        data_.file                  = map["fi"];
        data_.dir                   = map["di"];
        data_.link                  = map["ln"];
        data_.exec                  = map["ex"];

        for (const auto& [k, v] : map) {
            if (k.starts_with('*')) {
                // strip * from key.
                // We keep the dot since fs::path::extension() includes the dot.
                extensions_.emplace(k.substr(1), v);
            }
        }
        return true;
    }

    ls_colors_data data_;
    style_map extensions_;
};
