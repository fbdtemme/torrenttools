
#include <catch2/catch_all.hpp>
#include "ls_colors.hpp"

TEST_CASE("test parse_color")
{
    SECTION("terminal color - foreground") {
        auto input = "35";
        auto [c, it] = detail::parse_color(input);
        CHECK(c.get_foreground_color() == tc::terminal_color(35));
    }
    SECTION("terminal color - background") {
        auto input = "43";
        auto [c, it] = detail::parse_color(input);
        CHECK(c.get_background_color() == tc::terminal_color(33));
    }
    SECTION("256-bit terminal color - foreground") {
        auto input = "38;5;156";
        auto [c, it] = detail::parse_color(input);
        CHECK(c.get_foreground_color() == tc::terminal_color_256(156));
    }
    SECTION("256-bit terminal color - background") {
        auto input = "48;5;156";
        auto [c, it] = detail::parse_color(input);
        CHECK(c.get_background_color() == tc::terminal_color_256(156));
    }
    SECTION("RGB color -foreground") {
        auto input = "38;2;4;194;130";
        auto [c, it] = detail::parse_color(input);
        CHECK(c.get_foreground_color() == tc::rgb_color(4, 194, 130));
    }
    SECTION("RGB color - background") {
        auto input = "48;2;4;194;130";
        auto [c, it] = detail::parse_color(input);
        CHECK(c.get_background_color() == tc::rgb_color(4, 194, 130));
    }
    SECTION("missing color after alternate color mode") {
        auto input1 = "38;5";
        CHECK_THROWS(detail::parse_color(input1));
        auto input2 = "38;2";
        CHECK_THROWS(detail::parse_color(input2));
    }
    SECTION("missing rgb components") {
        auto input1 = "38;2;13";
        CHECK_THROWS(detail::parse_color(input1));
        auto input2= "38;2;13;234";
        CHECK_THROWS(detail::parse_color(input2));
    }
    SECTION("invalid terminal color - out of range") {
        auto input = "1243";
        CHECK_THROWS(detail::parse_color(input));
    }
    SECTION("invalid 256-bit terminal color - out of range") {
        auto input = "38;5;1243";
        CHECK_THROWS(detail::parse_color(input));
    }
    SECTION("invalid RGB color - out of range") {
        auto input = "38;2;20;156;399";
        CHECK_THROWS(detail::parse_color(input));
    }
    SECTION("invalid RGB color - missing red component") {
        auto input = "38;2";
        CHECK_THROWS(detail::parse_color(input));
    }
    SECTION("invalid RGB color - missing green component") {
        auto input = "38;2;20;";
        CHECK_THROWS(detail::parse_color(input));
    }
    SECTION("invalid RGB color - missing blue component") {
        auto input = "38;2;20;156";
        CHECK_THROWS(detail::parse_color(input));
    }
    SECTION("invalid argument seperator") {
        auto input = "38|40";
        CHECK_THROWS(detail::parse_color(input));
    }
    SECTION("expected value after alternate color mode") {
        auto input = "38;5";
        CHECK_THROWS(detail::parse_color(input));
    }
}


TEST_CASE("test parse_ls_colors - 256 bit colors")
{
    SECTION("default") {
        auto input = "rs=0:di=01;34:ln=01;36:mh=00:pi=40;33:so=01;35:do=01;35:bd=40;33;01:cd=40;33;01:or=40;31;01:mi=00:su=37;41:sg=30;43:ca=30;41:tw=30;42:ow=34;42:st=37;44:ex=01;32:*.tar=01;31:*.tgz=01;31:*.arc=01;31:*.arj=01;31:*.taz=01;31:*.lha=01;31:*.lz4=01;31:*.lzh=01;31:*.lzma=01;31:*.tlz=01;31:*.txz=01;31:*.tzo=01;31:*.t7z=01;31:*.zip=01;31:*.z=01;31:*.dz=01;31:*.gz=01;31:*.lrz=01;31:*.lz=01;31:*.lzo=01;31:*.xz=01;31:*.zst=01;31:*.tzst=01;31:*.bz2=01;31:*.bz=01;31:*.tbz=01;31:*.tbz2=01;31:*.tz=01;31:*.deb=01;31:*.rpm=01;31:*.jar=01;31:*.war=01;31:*.ear=01;31:*.sar=01;31:*.rar=01;31:*.alz=01;31:*.ace=01;31:*.zoo=01;31:*.cpio=01;31:*.7z=01;31:*.rz=01;31:*.cab=01;31:*.wim=01;31:*.swm=01;31:*.dwm=01;31:*.esd=01;31:*.jpg=01;35:*.jpeg=01;35:*.mjpg=01;35:*.mjpeg=01;35:*.gif=01;35:*.bmp=01;35:*.pbm=01;35:*.pgm=01;35:*.ppm=01;35:*.tga=01;35:*.xbm=01;35:*.xpm=01;35:*.tif=01;35:*.tiff=01;35:*.png=01;35:*.svg=01;35:*.svgz=01;35:*.mng=01;35:*.pcx=01;35:*.mov=01;35:*.mpg=01;35:*.mpeg=01;35:*.m2v=01;35:*.mkv=01;35:*.webm=01;35:*.webp=01;35:*.ogm=01;35:*.mp4=01;35:*.m4v=01;35:*.mp4v=01;35:*.vob=01;35:*.qt=01;35:*.nuv=01;35:*.wmv=01;35:*.asf=01;35:*.rm=01;35:*.rmvb=01;35:*.flc=01;35:*.avi=01;35:*.fli=01;35:*.flv=01;35:*.gl=01;35:*.dl=01;35:*.xcf=01;35:*.xwd=01;35:*.yuv=01;35:*.cgm=01;35:*.emf=01;35:*.ogv=01;35:*.ogx=01;35:*.aac=00;36:*.au=00;36:*.flac=00;36:*.m4a=00;36:*.mid=00;36:*.midi=00;36:*.mka=00;36:*.mp3=00;36:*.mpc=00;36:*.ogg=00;36:*.ra=00;36:*.wav=00;36:*.oga=00;36:*.opus=00;36:*.spx=00;36:*.xspf=00;36:";
        detail::parse_ls_colors(input);
    }
    SECTION("8 bit colors") {
        auto input = "rs=0:di=38;5;27:ln=38;5;51:mh=44;38;5;15:pi=40;38;5;11:so=38;5;13:do=38;5;5:bd=48;5;232;38;5;11:cd=48;5;232;38;5;3:or=48;5;232;38;5;9:mi=05;48;5;232;38;5;15:su=48;5;196;38;5;15:sg=48;5;11;38;5;16:ca=48;5;196;38;5;226:tw=48;5;10;38;5;16:ow=48;5;10;38;5;21:st=48;5;21;38;5;15:ex=38;5;34:*.tar=38;5;9:*.tgz=38;5;9:*.arc=38;5;9:*.arj=38;5;9:*.taz=38;5;9:*.lha=38;5;9:*.lz4=38;5;9:*.lzh=38;5;9:*.lzma=38;5;9:*.tlz=38;5;9:*.txz=38;5;9:*.tzo=38;5;9:*.t7z=38;5;9:*.zip=38;5;9:*.z=38;5;9:*.Z=38;5;9:*.dz=38;5;9:*.gz=38;5;9:*.lrz=38;5;9:*.lz=38;5;9:*.lzo=38;5;9:*.xz=38;5;9:*.bz2=38;5;9:*.bz=38;5;9:*.tbz=38;5;9:*.tbz2=38;5;9:*.tz=38;5;9:*.deb=38;5;9:*.rpm=38;5;9:*.jar=38;5;9:*.war=38;5;9:*.ear=38;5;9:*.sar=38;5;9:*.rar=38;5;9:*.alz=38;5;9:*.ace=38;5;9:*.zoo=38;5;9:*.cpio=38;5;9:*.7z=38;5;9:*.rz=38;5;9:*.cab=38;5;9:*.jpg=38;5;13:*.jpeg=38;5;13:*.gif=38;5;13:*.bmp=38;5;13:*.pbm=38;5;13:*.pgm=38;5;13:*.ppm=38;5;13:*.tga=38;5;13:*.xbm=38;5;13:*.xpm=38;5;13:*.tif=38;5;13:*.tiff=38;5;13:*.png=38;5;13:*.svg=38;5;13:*.svgz=38;5;13:*.mng=38;5;13:*.pcx=38;5;13:*.mov=38;5;13:*.mpg=38;5;13:*.mpeg=38;5;13:*.m2v=38;5;13:*.mkv=38;5;13:*.webm=38;5;13:*.ogm=38;5;13:*.mp4=38;5;13:*.m4v=38;5;13:*.mp4v=38;5;13:*.vob=38;5;13:*.qt=38;5;13:*.nuv=38;5;13:*.wmv=38;5;13:*.asf=38;5;13:*.rm=38;5;13:*.rmvb=38;5;13:*.flc=38;5;13:*.avi=38;5;13:*.fli=38;5;13:*.flv=38;5;13:*.gl=38;5;13:*.dl=38;5;13:*.xcf=38;5;13:*.xwd=38;5;13:*.yuv=38;5;13:*.cgm=38;5;13:*.emf=38;5;13:*.axv=38;5;13:*.anx=38;5;13:*.ogv=38;5;13:*.ogx=38;5;13:*.aac=38;5;45:*.au=38;5;45:*.flac=38;5;45:*.mid=38;5;45:*.midi=38;5;45:*.mka=38;5;45:*.mp3=38;5;45:*.mpc=38;5;45:*.ogg=38;5;45:*.ra=38;5;45:*.wav=38;5;45:*.axa=38;5;45:*.oga=38;5;45:*.spx=38;5;45:*.xspf=38;5;45";
        detail::parse_ls_colors(input);
    }
    SECTION("24 bit colors") {
        auto input = "di=1;38;2;69;133;136";
        detail::parse_ls_colors(input);
    }
}