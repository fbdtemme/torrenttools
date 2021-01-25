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

namespace fs = std::filesystem;

constexpr auto default_colors = (
        "rs=0:di=01;34:ln=01;36:mh=00:pi=40;33:so=01;35:do=01;35:bd=40;33;01:cd=40;33;01:or=40;31;01:"
        "mi=00:su=37;41:sg=30;43:ca=30;41:tw=30;42:ow=34;42:st=37;44:ex=01;32:*.tar=01;31:*.tgz=01;31:"
        "*.arc=01;31:*.arj=01;31:*.taz=01;31:*.lha=01;31:*.lz4=01;31:*.lzh=01;31:*.lzma=01;31:*.tlz=01;31:"
        "*.txz=01;31:*.tzo=01;31:*.t7z=01;31:*.zip=01;31:*.z=01;31:*.dz=01;31:*.gz=01;31:*.lrz=01;31:*.lz=01;31:"
        "*.lzo=01;31:*.xz=01;31:*.zst=01;31:*.tzst=01;31:*.bz2=01;31:*.bz=01;31:*.tbz=01;31:*.tbz2=01;31:"
        "*.tz=01;31:*.deb=01;31:*.rpm=01;31:*.jar=01;31:*.war=01;31:*.ear=01;31:*.sar=01;31:*.rar=01;31:*.alz=01;31:"
        "*.ace=01;31:*.zoo=01;31:*.cpio=01;31:*.7z=01;31:*.rz=01;31:*.cab=01;31:*.wim=01;31:*.swm=01;31:*.dwm=01;31:"
        "*.esd=01;31:*.jpg=01;35:*.jpeg=01;35:*.mjpg=01;35:*.mjpeg=01;35:*.gif=01;35:*.bmp=01;35:*.pbm=01;35:"
        "*.pgm=01;35:*.ppm=01;35:*.tga=01;35:*.xbm=01;35:*.xpm=01;35:*.tif=01;35:*.tiff=01;35:*.png=01;35:"
        "*.svg=01;35:*.svgz=01;35:*.mng=01;35:*.pcx=01;35:*.mov=01;35:*.mpg=01;35:*.mpeg=01;35:*.m2v=01;35:"
        "*.mkv=01;35:*.webm=01;35:*.ogm=01;35:*.mp4=01;35:*.m4v=01;35:*.mp4v=01;35:*.vob=01;35:*.qt=01;35:"
        "*.nuv=01;35:*.wmv=01;35:*.asf=01;35:*.rm=01;35:*.rmvb=01;35:*.flc=01;35:*.avi=01;35:*.fli=01;35:"
        "*.flv=01;35:*.gl=01;35:*.dl=01;35:*.xcf=01;35:*.xwd=01;35:*.yuv=01;35:*.cgm=01;35:*.emf=01;35:"
        "*.ogv=01;35:*.ogx=01;35:*.aac=00;36:*.au=00;36:*.flac=00;36:*.m4a=00;36:*.mid=00;36:*.midi=00;36:"
        "*.mka=00;36:*.mp3=00;36:*.mpc=00;36:*.ogg=00;36:*.ra=00;36:*.wav=00;36:*.oga=00;36:*.opus=00;36:"
        "*.spx=00;36:*.xspf=00;36:';"
);

namespace detail {

inline auto parse_ls_colors(std::string_view ls_colors) -> std::unordered_map<std::string, fmt::text_style>
{
    std::unordered_map<std::string, fmt::text_style> out{};
    std::size_t prev = 0, pos = 0, idx = 0, sep_prev = 0, sep = 0;

    std::uint8_t emphasis;
    std::uint8_t foreground;
    std::uint8_t background;

    while ((pos = ls_colors.find(":", prev)) != std::string_view::npos) {
        auto entry = ls_colors.substr(prev, pos-prev);
        std::size_t equal_sign_pos = entry.find('=');
        auto key = entry.substr(0, equal_sign_pos);
        auto value = entry.substr(equal_sign_pos+1);

        emphasis = 0;
        foreground = static_cast<std::uint8_t>(fmt::terminal_color::black);
        background = 49;

        if ((sep = value.find(";")) != std::string_view::npos) {
            auto style_str = value.substr(0, sep);

            if (auto [p, ec] = std::from_chars(style_str.begin(), style_str.end(), emphasis);
                    ec != std::errc()) {
                continue;
            }
            sep_prev = sep+1;

            if ((sep = value.substr(sep_prev).find(";")) != std::string_view::npos) {
                sep_prev = sep+1;
                if ((sep = value.substr(sep_prev).find(";")) != std::string_view::npos) {
                    auto background_str = value.substr(sep_prev, sep-sep_prev);

                    if (auto [p, ec] = std::from_chars(background_str.begin(), background_str.end(), background);
                            ec != std::errc()) {
                        continue;
                    }
                }
            }
            else {
                auto foreground_str = value.substr(sep_prev, sep-sep_prev);

                if (auto [p, ec] = std::from_chars(foreground_str.begin(), foreground_str.end(), foreground);
                        ec != std::errc()) {
                    continue;
                }
            }
        }
        prev = pos+1;
        fmt::text_style style {};
        out.insert_or_assign(std::string(key), (
                fmt::text_style(fmt::emphasis(emphasis ? emphasis <= 4 : 0)) |
                        fg(fmt::terminal_color(foreground)) |
                        bg(fmt::terminal_color(background))
                        ));

    }
    return out;
}

} // namespace detail

struct ls_colors
{
    using style_type = fmt::text_style;
    using extension_map = std::unordered_map<std::string, style_type>;

    struct ls_colors_data
    {
        style_type normal;                 ///< Normal (nonfilename) text
        style_type file;                   ///< Regular file
        style_type dir;                    ///< Directory
        style_type link;                   ///< Symbolic link
        style_type fifo;                   ///< Named pipe (FIFO)
        style_type door;                   ///< Solaris Door
        style_type blk;                    ///< Block device
        style_type chr;                    ///< Character device
        style_type orphan;                 ///< Orphaned symbolic link
        style_type sock;                   ///< Socket
        style_type setuid;                 ///< File that is setuid (u+s)
        style_type setgid;                 ///< File that is setgid (g+s)
        style_type sticky_other_writable;  ///< Directory that is sticky and other-writable (+t,o+w)
        style_type other_writable;         ///< Directory that other-writable (+t,o+w) and not sticky
        style_type sticky;                 ///< Directory with the sticky bit set (+t) and not other-writable
        style_type exec;                   ///< Executable file
        style_type missing;                ///< Missing file
    };

    explicit ls_colors(bool system_colors = true)
        : data_()
        , extensions_()
    {
        bool success = false;

        if (system_colors) {
            success = load_system_colors();
        }
        if (!success) {
            load_default_colors();
        }
    }

    inline auto get_extension(const fs::path& name) -> fmt::text_style
    {
        const auto& ext = name.extension().string();

        if (auto it = extensions_.find(ext); it != extensions_.end()) {
            return it->second;
        }
        return {};
    }

private:
    bool load_system_colors() noexcept
    {
        const char* lc_colors_p = std::getenv("LS_COLORS");
        if (!lc_colors_p)
            return false;

        auto map = detail::parse_ls_colors(lc_colors_p);
        extensions_ = std::move(map);
    }

    void load_default_colors() noexcept
    {
        auto map = detail::parse_ls_colors(default_colors);
        load_lc_colors_data(map);
        extensions_ = std::move(map);
    }

    void load_lc_colors_data(std::unordered_map<std::string, fmt::text_style>& map)
    {
        data_.normal                = map["no"];
        data_.file                  = map["fi"];
        data_.dir                   = map["di"];
        data_.link                  = map["ln"];
        data_.fifo                  = map["pi"];
        data_.door                  = map["do"];
        data_.blk                   = map["bd"];
        data_.chr                   = map["cd"];
        data_.orphan                = map["or"];
        data_.sock                  = map["so"];
        data_.setuid                = map["su"];
        data_.setgid                = map["sg"];
        data_.sticky_other_writable = map["tw"];
        data_.other_writable        = map["ow"];
        data_.sticky                = map["st"];
        data_.exec                  = map["ex"];
        data_.missing               = map["mi"];
    }

    ls_colors_data data_;
    extension_map extensions_;
};


using namespace std::string_view_literals;

// TODO: switch to termcontrol::text_style
inline auto load_ls_style_map() -> std::map<std::string_view, termcontrol::text_style>
{
    using namespace std::string_literals;
    std::map<std::string_view, termcontrol::text_style> ls_style_map {};
    const char* env_p = std::getenv("LS_COLORS");

    if (!env_p)
        return {};

    std::string_view ls_colors(env_p);
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

        if ((sep = value.find(';')) != std::string_view::npos) {
            auto style_str = value.substr(0, sep);

            if (auto[p, ec] = std::from_chars(style_str.begin(), style_str.end(), emphasis);
                    ec != std::errc()) {
                continue;
            }
            sep_prev = sep + 1;

            if ((sep = value.substr(sep_prev).find(';')) != std::string_view::npos) {
                sep_prev = sep + 1;
                if ((sep = value.substr(sep_prev).find(';')) != std::string_view::npos) {
                    auto background_str = value.substr(sep_prev, sep-sep_prev);

                    if (auto[p, ec] = std::from_chars(background_str.begin(), background_str.end(), background);
                            ec != std::errc()) {
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
        ls_style_map.insert_or_assign(key, style);
    }
    return ls_style_map;
}
