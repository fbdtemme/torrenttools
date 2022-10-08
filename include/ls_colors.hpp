#pragma once

#include <cstdlib>
#include <array>
#include <string>
#include <type_traits>
#include <string_view>
#include <cmath>
#include <charconv>
#include <map>
#include <ranges>

#include <filesystem>

#include <fmt/format.h>
#include <fmt/color.h>

#include <termcontrol/detail/fmt.hpp>
#include <dottorrent/general.hpp>
#include <dottorrent/file_entry.hpp>

namespace fs = std::filesystem;
namespace rng = std::ranges;
namespace tc = termcontrol;

namespace detail {

std::pair<tc::rgb_color, std::size_t>
parse_rgb_color_component(std::string_view rgb_component);

std::pair<tc::text_style, std::size_t>
parse_color(std::string_view arguments);

tc::emphasis
parse_emphasis(std::uint8_t graphics_attribute);

std::map<std::string, termcontrol::text_style>
parse_ls_colors(std::string_view ls_colors);

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

    explicit ls_colors();

    termcontrol::text_style file_style(const dottorrent::file_entry& entry) const;

    termcontrol::text_style directory_style() const;

private:
    bool load_system_colors() noexcept;

    ls_colors_data data_;
    style_map extensions_;
};
