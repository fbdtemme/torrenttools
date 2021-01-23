
#pragma once
#include <array>
#include <string_view>
#include <cstdint>
#include <span>


namespace cliprogress {

struct animation_style
{
    std::span<const std::string_view> frames;
    std::uint16_t interval;
};

namespace animations {

using namespace std::string_view_literals;

inline constexpr std::array dots_frames = {
        "⠋"sv,
        "⠙"sv,
        "⠹"sv,
        "⠸"sv,
        "⠼"sv,
        "⠴"sv,
        "⠦"sv,
        "⠧"sv,
        "⠇"sv,
        "⠏"sv
};

inline constexpr animation_style dots = {
    .frames = dots_frames,
    .interval = 80,
};

inline constexpr std::array dots2_frames = {
        "⣾"sv,
        "⣽"sv,
        "⣻"sv,
        "⢿"sv,
        "⡿"sv,
        "⣟"sv,
        "⣯"sv,
        "⣷"sv
};

inline constexpr animation_style dots2 = {
    .frames = dots2_frames,
    .interval = 80,
};

inline constexpr std::array dots3_frames = {
        "⠋"sv,
        "⠙"sv,
        "⠚"sv,
        "⠞"sv,
        "⠖"sv,
        "⠦"sv,
        "⠴"sv,
        "⠲"sv,
        "⠳"sv,
        "⠓"sv
};

inline constexpr animation_style dots3 = {
    .frames = dots3_frames,
    .interval = 80,
};

inline constexpr std::array dots4_frames = {
        "⠄"sv,
        "⠆"sv,
        "⠇"sv,
        "⠋"sv,
        "⠙"sv,
        "⠸"sv,
        "⠰"sv,
        "⠠"sv,
        "⠰"sv,
        "⠸"sv,
        "⠙"sv,
        "⠋"sv,
        "⠇"sv,
        "⠆"sv
};

inline constexpr animation_style dots4 = {
    .frames = dots4_frames,
    .interval = 80,
};

inline constexpr std::array dots5_frames = {
        "⠋"sv,
        "⠙"sv,
        "⠚"sv,
        "⠒"sv,
        "⠂"sv,
        "⠂"sv,
        "⠒"sv,
        "⠲"sv,
        "⠴"sv,
        "⠦"sv,
        "⠖"sv,
        "⠒"sv,
        "⠐"sv,
        "⠐"sv,
        "⠒"sv,
        "⠓"sv,
        "⠋"sv
};

inline constexpr animation_style dots5 = {
    .frames = dots5_frames,
    .interval = 80,
};

inline constexpr std::array dots6_frames = {
        "⠁"sv,
        "⠉"sv,
        "⠙"sv,
        "⠚"sv,
        "⠒"sv,
        "⠂"sv,
        "⠂"sv,
        "⠒"sv,
        "⠲"sv,
        "⠴"sv,
        "⠤"sv,
        "⠄"sv,
        "⠄"sv,
        "⠤"sv,
        "⠴"sv,
        "⠲"sv,
        "⠒"sv,
        "⠂"sv,
        "⠂"sv,
        "⠒"sv,
        "⠚"sv,
        "⠙"sv,
        "⠉"sv,
        "⠁"sv
};

inline constexpr animation_style dots6 = {
    .frames = dots6_frames,
    .interval = 80,
};

inline constexpr std::array dots7_frames = {
        "⠈"sv,
        "⠉"sv,
        "⠋"sv,
        "⠓"sv,
        "⠒"sv,
        "⠐"sv,
        "⠐"sv,
        "⠒"sv,
        "⠖"sv,
        "⠦"sv,
        "⠤"sv,
        "⠠"sv,
        "⠠"sv,
        "⠤"sv,
        "⠦"sv,
        "⠖"sv,
        "⠒"sv,
        "⠐"sv,
        "⠐"sv,
        "⠒"sv,
        "⠓"sv,
        "⠋"sv,
        "⠉"sv,
        "⠈"sv
};

inline constexpr animation_style dots7 = {
    .frames = dots7_frames,
    .interval = 80,
};

inline constexpr std::array dots8_frames = {
        "⠁"sv,
        "⠁"sv,
        "⠉"sv,
        "⠙"sv,
        "⠚"sv,
        "⠒"sv,
        "⠂"sv,
        "⠂"sv,
        "⠒"sv,
        "⠲"sv,
        "⠴"sv,
        "⠤"sv,
        "⠄"sv,
        "⠄"sv,
        "⠤"sv,
        "⠠"sv,
        "⠠"sv,
        "⠤"sv,
        "⠦"sv,
        "⠖"sv,
        "⠒"sv,
        "⠐"sv,
        "⠐"sv,
        "⠒"sv,
        "⠓"sv,
        "⠋"sv,
        "⠉"sv,
        "⠈"sv,
        "⠈"sv
};

inline constexpr animation_style dots8 = {
    .frames = dots8_frames,
    .interval = 80,
};

inline constexpr std::array dots9_frames = {
        "⢹"sv,
        "⢺"sv,
        "⢼"sv,
        "⣸"sv,
        "⣇"sv,
        "⡧"sv,
        "⡗"sv,
        "⡏"sv
};

inline constexpr animation_style dots9 = {
    .frames = dots9_frames,
    .interval = 80,
};

inline constexpr std::array dots10_frames = {
        "⢄"sv,
        "⢂"sv,
        "⢁"sv,
        "⡁"sv,
        "⡈"sv,
        "⡐"sv,
        "⡠"sv
};

inline constexpr animation_style dots10 = {
    .frames = dots10_frames,
    .interval = 80,
};

inline constexpr std::array dots11_frames = {
        "⠁"sv,
        "⠂"sv,
        "⠄"sv,
        "⡀"sv,
        "⢀"sv,
        "⠠"sv,
        "⠐"sv,
        "⠈"sv
};

inline constexpr animation_style dots11 = {
    .frames = dots11_frames,
    .interval = 100,
};

inline constexpr std::array dots12_frames = {
        "⢀⠀"sv,
        "⡀⠀"sv,
        "⠄⠀"sv,
        "⢂⠀"sv,
        "⡂⠀"sv,
        "⠅⠀"sv,
        "⢃⠀"sv,
        "⡃⠀"sv,
        "⠍⠀"sv,
        "⢋⠀"sv,
        "⡋⠀"sv,
        "⠍⠁"sv,
        "⢋⠁"sv,
        "⡋⠁"sv,
        "⠍⠉"sv,
        "⠋⠉"sv,
        "⠋⠉"sv,
        "⠉⠙"sv,
        "⠉⠙"sv,
        "⠉⠩"sv,
        "⠈⢙"sv,
        "⠈⡙"sv,
        "⢈⠩"sv,
        "⡀⢙"sv,
        "⠄⡙"sv,
        "⢂⠩"sv,
        "⡂⢘"sv,
        "⠅⡘"sv,
        "⢃⠨"sv,
        "⡃⢐"sv,
        "⠍⡐"sv,
        "⢋⠠"sv,
        "⡋⢀"sv,
        "⠍⡁"sv,
        "⢋⠁"sv,
        "⡋⠁"sv,
        "⠍⠉"sv,
        "⠋⠉"sv,
        "⠋⠉"sv,
        "⠉⠙"sv,
        "⠉⠙"sv,
        "⠉⠩"sv,
        "⠈⢙"sv,
        "⠈⡙"sv,
        "⠈⠩"sv,
        "⠀⢙"sv,
        "⠀⡙"sv,
        "⠀⠩"sv,
        "⠀⢘"sv,
        "⠀⡘"sv,
        "⠀⠨"sv,
        "⠀⢐"sv,
        "⠀⡐"sv,
        "⠀⠠"sv,
        "⠀⢀"sv,
        "⠀⡀"sv
};

inline constexpr animation_style dots12 = {
    .frames = dots12_frames,
    .interval = 80,
};

inline constexpr std::array line_frames = {
        "-"sv,
        "\\"sv,
        "|"sv,
        "/"sv
};

inline constexpr animation_style line = {
    .frames = line_frames,
    .interval = 130,
};

inline constexpr std::array line2_frames = {
        "⠂"sv,
        "-"sv,
        "–"sv,
        "—"sv,
        "–"sv,
        "-"sv
};

inline constexpr animation_style line2 = {
    .frames = line2_frames,
    .interval = 100,
};

inline constexpr std::array pipe_frames = {
        "┤"sv,
        "┘"sv,
        "┴"sv,
        "└"sv,
        "├"sv,
        "┌"sv,
        "┬"sv,
        "┐"sv
};

inline constexpr animation_style pipe = {
    .frames = pipe_frames,
    .interval = 100,
};

inline constexpr std::array simple_dots_frames = {
        ".  "sv,
        ".. "sv,
        "..."sv,
        "   "sv
};

inline constexpr animation_style simple_dots = {
    .frames = simple_dots_frames,
    .interval = 400,
};

inline constexpr std::array simple_dots_scrolling_frames = {
        ".  "sv,
        ".. "sv,
        "..."sv,
        " .."sv,
        "  ."sv,
        "   "sv
};

inline constexpr animation_style simple_dots_scrolling = {
    .frames = simple_dots_scrolling_frames,
    .interval = 200,
};

inline constexpr std::array star_frames = {
        "✶"sv,
        "✸"sv,
        "✹"sv,
        "✺"sv,
        "✹"sv,
        "✷"sv
};

inline constexpr animation_style star = {
    .frames = star_frames,
    .interval = 70,
};

inline constexpr std::array star2_frames = {
        "+"sv,
        "x"sv,
        "*"sv
};

inline constexpr animation_style star2 = {
    .frames = star2_frames,
    .interval = 80,
};

inline constexpr std::array flip_frames = {
        "_"sv,
        "_"sv,
        "_"sv,
        "-"sv,
        "`"sv,
        "`"sv,
        "'"sv,
        "´"sv,
        "-"sv,
        "_"sv,
        "_"sv,
        "_"sv
};

inline constexpr animation_style flip = {
    .frames = flip_frames,
    .interval = 70,
};

inline constexpr std::array hamburger_frames = {
        "☱"sv,
        "☲"sv,
        "☴"sv
};

inline constexpr animation_style hamburger = {
    .frames = hamburger_frames,
    .interval = 100,
};

inline constexpr std::array grow_vertical_frames = {
        "▁"sv,
        "▃"sv,
        "▄"sv,
        "▅"sv,
        "▆"sv,
        "▇"sv,
        "▆"sv,
        "▅"sv,
        "▄"sv,
        "▃"sv
};

inline constexpr animation_style grow_vertical = {
    .frames = grow_vertical_frames,
    .interval = 120,
};

inline constexpr std::array grow_horizontal_frames = {
        "▏"sv,
        "▎"sv,
        "▍"sv,
        "▌"sv,
        "▋"sv,
        "▊"sv,
        "▉"sv,
        "▊"sv,
        "▋"sv,
        "▌"sv,
        "▍"sv,
        "▎"sv
};

inline constexpr animation_style grow_horizontal = {
    .frames = grow_horizontal_frames,
    .interval = 120,
};

inline constexpr std::array balloon_frames = {
        " "sv,
        "."sv,
        "o"sv,
        "O"sv,
        "@"sv,
        "*"sv,
        " "sv
};

inline constexpr animation_style balloon = {
    .frames = balloon_frames,
    .interval = 140,
};

inline constexpr std::array balloon2_frames = {
        "."sv,
        "o"sv,
        "O"sv,
        "°"sv,
        "O"sv,
        "o"sv,
        "."sv
};

inline constexpr animation_style balloon2 = {
    .frames = balloon2_frames,
    .interval = 120,
};

inline constexpr std::array noise_frames = {
        "▓"sv,
        "▒"sv,
        "░"sv
};

inline constexpr animation_style noise = {
    .frames = noise_frames,
    .interval = 100,
};

inline constexpr std::array bounce_frames = {
        "⠁"sv,
        "⠂"sv,
        "⠄"sv,
        "⠂"sv
};

inline constexpr animation_style bounce = {
    .frames = bounce_frames,
    .interval = 120,
};

inline constexpr std::array box_bounce_frames = {
        "▖"sv,
        "▘"sv,
        "▝"sv,
        "▗"sv
};

inline constexpr animation_style box_bounce = {
    .frames = box_bounce_frames,
    .interval = 120,
};

inline constexpr std::array box_bounce2_frames = {
        "▌"sv,
        "▀"sv,
        "▐"sv,
        "▄"sv
};

inline constexpr animation_style box_bounce2 = {
    .frames = box_bounce2_frames,
    .interval = 100,
};

inline constexpr std::array triangle_frames = {
        "◢"sv,
        "◣"sv,
        "◤"sv,
        "◥"sv
};

inline constexpr animation_style triangle = {
    .frames = triangle_frames,
    .interval = 50,
};

inline constexpr std::array arc_frames = {
        "◜"sv,
        "◠"sv,
        "◝"sv,
        "◞"sv,
        "◡"sv,
        "◟"sv
};

inline constexpr animation_style arc = {
    .frames = arc_frames,
    .interval = 100,
};

inline constexpr std::array circle_frames = {
        "◡"sv,
        "⊙"sv,
        "◠"sv
};

inline constexpr animation_style circle = {
    .frames = circle_frames,
    .interval = 120,
};

inline constexpr std::array square_corners_frames = {
        "◰"sv,
        "◳"sv,
        "◲"sv,
        "◱"sv
};

inline constexpr animation_style square_corners = {
    .frames = square_corners_frames,
    .interval = 180,
};

inline constexpr std::array circle_quarters_frames = {
        "◴"sv,
        "◷"sv,
        "◶"sv,
        "◵"sv
};

inline constexpr animation_style circle_quarters = {
    .frames = circle_quarters_frames,
    .interval = 120,
};

inline constexpr std::array circle_halves_frames = {
        "◐"sv,
        "◓"sv,
        "◑"sv,
        "◒"sv
};

inline constexpr animation_style circle_halves = {
    .frames = circle_halves_frames,
    .interval = 50,
};

inline constexpr std::array squish_frames = {
        "╫"sv,
        "╪"sv
};

inline constexpr animation_style squish = {
    .frames = squish_frames,
    .interval = 100,
};

inline constexpr std::array toggle_frames = {
        "⊶"sv,
        "⊷"sv
};

inline constexpr animation_style toggle = {
    .frames = toggle_frames,
    .interval = 250,
};

inline constexpr std::array toggle2_frames = {
        "▫"sv,
        "▪"sv
};

inline constexpr animation_style toggle2 = {
    .frames = toggle2_frames,
    .interval = 80,
};

inline constexpr std::array toggle3_frames = {
        "□"sv,
        "■"sv
};

inline constexpr animation_style toggle3 = {
    .frames = toggle3_frames,
    .interval = 120,
};

inline constexpr std::array toggle4_frames = {
        "■"sv,
        "□"sv,
        "▪"sv,
        "▫"sv
};

inline constexpr animation_style toggle4 = {
    .frames = toggle4_frames,
    .interval = 100,
};

inline constexpr std::array toggle5_frames = {
        "▮"sv,
        "▯"sv
};

inline constexpr animation_style toggle5 = {
    .frames = toggle5_frames,
    .interval = 100,
};

inline constexpr std::array toggle6_frames = {
        "ဝ"sv,
        "၀"sv
};

inline constexpr animation_style toggle6 = {
    .frames = toggle6_frames,
    .interval = 300,
};

inline constexpr std::array toggle7_frames = {
        "⦾"sv,
        "⦿"sv
};

inline constexpr animation_style toggle7 = {
    .frames = toggle7_frames,
    .interval = 80,
};

inline constexpr std::array toggle8_frames = {
        "◍"sv,
        "◌"sv
};

inline constexpr animation_style toggle8 = {
    .frames = toggle8_frames,
    .interval = 100,
};

inline constexpr std::array toggle9_frames = {
        "◉"sv,
        "◎"sv
};

inline constexpr animation_style toggle9 = {
    .frames = toggle9_frames,
    .interval = 100,
};

inline constexpr std::array toggle10_frames = {
        "㊂"sv,
        "㊀"sv,
        "㊁"sv
};

inline constexpr animation_style toggle10 = {
    .frames = toggle10_frames,
    .interval = 100,
};

inline constexpr std::array toggle11_frames = {
        "⧇"sv,
        "⧆"sv
};

inline constexpr animation_style toggle11 = {
    .frames = toggle11_frames,
    .interval = 50,
};

inline constexpr std::array toggle12_frames = {
        "☗"sv,
        "☖"sv
};

inline constexpr animation_style toggle12 = {
    .frames = toggle12_frames,
    .interval = 120,
};

inline constexpr std::array toggle13_frames = {
        "="sv,
        "*"sv,
        "-"sv
};

inline constexpr animation_style toggle13 = {
    .frames = toggle13_frames,
    .interval = 80,
};

inline constexpr std::array arrow_frames = {
        "←"sv,
        "↖"sv,
        "↑"sv,
        "↗"sv,
        "→"sv,
        "↘"sv,
        "↓"sv,
        "↙"sv
};

inline constexpr animation_style arrow = {
    .frames = arrow_frames,
    .interval = 100,
};

inline constexpr std::array arrow2_frames = {
        "⬆️ "sv,
        "↗️ "sv,
        "➡️ "sv,
        "↘️ "sv,
        "⬇️ "sv,
        "↙️ "sv,
        "⬅️ "sv,
        "↖️ "sv
};

inline constexpr animation_style arrow2 = {
    .frames = arrow2_frames,
    .interval = 80,
};

inline constexpr std::array arrow3_frames = {
        "▹▹▹▹▹"sv,
        "▸▹▹▹▹"sv,
        "▹▸▹▹▹"sv,
        "▹▹▸▹▹"sv,
        "▹▹▹▸▹"sv,
        "▹▹▹▹▸"sv
};

inline constexpr animation_style arrow3 = {
    .frames = arrow3_frames,
    .interval = 120,
};

inline constexpr std::array bouncing_bar_frames = {
        "[    ]"sv,
        "[=   ]"sv,
        "[==  ]"sv,
        "[=== ]"sv,
        "[ ===]"sv,
        "[  ==]"sv,
        "[   =]"sv,
        "[    ]"sv,
        "[   =]"sv,
        "[  ==]"sv,
        "[ ===]"sv,
        "[====]"sv,
        "[=== ]"sv,
        "[==  ]"sv,
        "[=   ]"sv
};

inline constexpr animation_style bouncing_bar = {
    .frames = bouncing_bar_frames,
    .interval = 80,
};

inline constexpr std::array bouncing_ball_frames = {
        "( ●    )"sv,
        "(  ●   )"sv,
        "(   ●  )"sv,
        "(    ● )"sv,
        "(     ●)"sv,
        "(    ● )"sv,
        "(   ●  )"sv,
        "(  ●   )"sv,
        "( ●    )"sv,
        "(●     )"sv
};

inline constexpr animation_style bouncing_ball = {
    .frames = bouncing_ball_frames,
    .interval = 80,
};

inline constexpr std::array smiley_frames = {
        "😄 "sv,
        "😝 "sv
};

inline constexpr animation_style smiley = {
    .frames = smiley_frames,
    .interval = 200,
};

inline constexpr std::array monkey_frames = {
        "🙈 "sv,
        "🙈 "sv,
        "🙉 "sv,
        "🙊 "sv
};

inline constexpr animation_style monkey = {
    .frames = monkey_frames,
    .interval = 300,
};

inline constexpr std::array hearts_frames = {
        "💛 "sv,
        "💙 "sv,
        "💜 "sv,
        "💚 "sv,
        "❤️ "sv
};

inline constexpr animation_style hearts = {
    .frames = hearts_frames,
    .interval = 100,
};

inline constexpr std::array clock_frames = {
        "🕛 "sv,
        "🕐 "sv,
        "🕑 "sv,
        "🕒 "sv,
        "🕓 "sv,
        "🕔 "sv,
        "🕕 "sv,
        "🕖 "sv,
        "🕗 "sv,
        "🕘 "sv,
        "🕙 "sv,
        "🕚 "sv
};

inline constexpr animation_style clock = {
    .frames = clock_frames,
    .interval = 100,
};

inline constexpr std::array earth_frames = {
        "🌍 "sv,
        "🌎 "sv,
        "🌏 "sv
};

inline constexpr animation_style earth = {
    .frames = earth_frames,
    .interval = 180,
};

inline constexpr std::array moon_frames = {
        "🌑 "sv,
        "🌒 "sv,
        "🌓 "sv,
        "🌔 "sv,
        "🌕 "sv,
        "🌖 "sv,
        "🌗 "sv,
        "🌘 "sv
};

inline constexpr animation_style moon = {
    .frames = moon_frames,
    .interval = 80,
};

inline constexpr std::array runner_frames = {
        "🚶 "sv,
        "🏃 "sv
};

inline constexpr animation_style runner = {
    .frames = runner_frames,
    .interval = 140,
};

inline constexpr std::array pong_frames = {
        "▐⠂       ▌"sv,
        "▐⠈       ▌"sv,
        "▐ ⠂      ▌"sv,
        "▐ ⠠      ▌"sv,
        "▐  ⡀     ▌"sv,
        "▐  ⠠     ▌"sv,
        "▐   ⠂    ▌"sv,
        "▐   ⠈    ▌"sv,
        "▐    ⠂   ▌"sv,
        "▐    ⠠   ▌"sv,
        "▐     ⡀  ▌"sv,
        "▐     ⠠  ▌"sv,
        "▐      ⠂ ▌"sv,
        "▐      ⠈ ▌"sv,
        "▐       ⠂▌"sv,
        "▐       ⠠▌"sv,
        "▐       ⡀▌"sv,
        "▐      ⠠ ▌"sv,
        "▐      ⠂ ▌"sv,
        "▐     ⠈  ▌"sv,
        "▐     ⠂  ▌"sv,
        "▐    ⠠   ▌"sv,
        "▐    ⡀   ▌"sv,
        "▐   ⠠    ▌"sv,
        "▐   ⠂    ▌"sv,
        "▐  ⠈     ▌"sv,
        "▐  ⠂     ▌"sv,
        "▐ ⠠      ▌"sv,
        "▐ ⡀      ▌"sv,
        "▐⠠       ▌"sv
};

inline constexpr animation_style pong = {
    .frames = pong_frames,
    .interval = 80,
};

inline constexpr std::array shark_frames = {
        "▐|\\____________▌"sv,
        "▐_|\\___________▌"sv,
        "▐__|\\__________▌"sv,
        "▐___|\\_________▌"sv,
        "▐____|\\________▌"sv,
        "▐_____|\\_______▌"sv,
        "▐______|\\______▌"sv,
        "▐_______|\\_____▌"sv,
        "▐________|\\____▌"sv,
        "▐_________|\\___▌"sv,
        "▐__________|\\__▌"sv,
        "▐___________|\\_▌"sv,
        "▐____________|\\▌"sv,
        "▐____________/|▌"sv,
        "▐___________/|_▌"sv,
        "▐__________/|__▌"sv,
        "▐_________/|___▌"sv,
        "▐________/|____▌"sv,
        "▐_______/|_____▌"sv,
        "▐______/|______▌"sv,
        "▐_____/|_______▌"sv,
        "▐____/|________▌"sv,
        "▐___/|_________▌"sv,
        "▐__/|__________▌"sv,
        "▐_/|___________▌"sv,
        "▐/|____________▌"sv
};

inline constexpr animation_style shark = {
    .frames = shark_frames,
    .interval = 120,
};

inline constexpr std::array dqpb_frames = {
        "d"sv,
        "q"sv,
        "p"sv,
        "b"sv
};

inline constexpr animation_style dqpb = {
    .frames = dqpb_frames,
    .interval = 100,
};

inline constexpr std::array weather_frames = {
        "☀️ "sv,
        "☀️ "sv,
        "☀️ "sv,
        "🌤 "sv,
        "⛅️ "sv,
        "🌥 "sv,
        "☁️ "sv,
        "🌧 "sv,
        "🌨 "sv,
        "🌧 "sv,
        "🌨 "sv,
        "🌧 "sv,
        "🌨 "sv,
        "⛈ "sv,
        "🌨 "sv,
        "🌧 "sv,
        "🌨 "sv,
        "☁️ "sv,
        "🌥 "sv,
        "⛅️ "sv,
        "🌤 "sv,
        "☀️ "sv,
        "☀️ "sv
};

inline constexpr animation_style weather = {
    .frames = weather_frames,
    .interval = 100,
};

inline constexpr std::array christmas_frames = {
        "🌲"sv,
        "🎄"sv
};

inline constexpr animation_style christmas = {
    .frames = christmas_frames,
    .interval = 400,
};

inline constexpr std::array grenade_frames = {
        "،   "sv,
        "′   "sv,
        " ´ "sv,
        " ‾ "sv,
        "  ⸌"sv,
        "  ⸊"sv,
        "  |"sv,
        "  ⁎"sv,
        "  ⁕"sv,
        " ෴ "sv,
        "  ⁓"sv,
        "   "sv,
        "   "sv,
        "   "sv
};

inline constexpr animation_style grenade = {
    .frames = grenade_frames,
    .interval = 80,
};

inline constexpr std::array point_frames = {
        "∙∙∙"sv,
        "●∙∙"sv,
        "∙●∙"sv,
        "∙∙●"sv,
        "∙∙∙"sv
};

inline constexpr animation_style point = {
    .frames = point_frames,
    .interval = 125,
};

inline constexpr std::array layer_frames = {
        "-"sv,
        "="sv,
        "≡"sv
};

inline constexpr animation_style layer = {
    .frames = layer_frames,
    .interval = 150,
};

inline constexpr std::array beta_wave_frames = {
        "ρββββββ"sv,
        "βρβββββ"sv,
        "ββρββββ"sv,
        "βββρβββ"sv,
        "ββββρββ"sv,
        "βββββρβ"sv,
        "ββββββρ"sv
};

inline constexpr animation_style beta_wave = {
    .frames = beta_wave_frames,
    .interval = 80,
};

} // namespace throbber_presets 
} // namespace cliprogress 
