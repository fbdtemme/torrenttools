#pragma once
#include <cstdint>
#include <variant>
#include <type_traits>
#include <bitset>
#include <optional>
#include <gsl-lite/gsl-lite.hpp>

#include "bitmask_operators.hpp"
#include "formatted_size.hpp"

/// Much code used from fmtlib
namespace termcontrol {

/// 4 bit colors
enum class terminal_color : std::uint8_t {
    black = 30,
    red,
    green,
    yellow,
    blue,
    magenta,
    cyan,
    white,
    reset = 39,
    bright_black = 90,
    bright_red,
    bright_green,
    bright_yellow,
    bright_blue,
    bright_magenta,
    bright_cyan,
    bright_white
};


TERMCONTROL_DEFINE_MAX_FORMATTED_SIZE(terminal_color, 2);

template <typename T>
struct enum_traits;

template <typename T>
concept rgb_color_enum =
        enum_traits<T>::is_color &&
        std::same_as<std::underlying_type_t<T>, std::uint32_t>;

#define TERMCONTROL_DEFINE_COLOR_ENUM(TYPE)  \
template <> struct enum_traits<TYPE> { static constexpr bool is_color = true; };

/// Color names as defined by CSS3
/// Note that these are a subset of the x11_colors.
/// Source: https://www.w3.org/TR/2018/REC-css-color-3-20180619/#hsla-color
enum class css_color : std::uint32_t {
    aliceblue	          = 0xF0F8FF,	  // RGB(240,248,255)
    antiquewhite	      = 0xFAEBD7,	  // RGB(250,235,215)
    aqua	              = 0x00FFFF,	  // RGB(0,255,255)
    aquamarine	          = 0x7FFFD4,	  // RGB(127,255,212)
    azure	              = 0xF0FFFF,	  // RGB(240,255,255)
    beige	              = 0xF5F5DC,     // RGB(245,245,220)
    bisque	              = 0xFFE4C4,     // RGB(255,228,196)
    black	              = 0x000000,     // RGB(0,0,0)
    blanchedalmond	      = 0xFFEBCD,     // RGB(255,235,205)
    blue	              = 0x0000FF,     // RGB(0,0,255)
    blueviolet	          = 0x8A2BE2,     // RGB(138,43,226)
    brown	              = 0xA52A2A,     // RGB(165,42,42)
    burlywood	          = 0xDEB887,     // RGB(222,184,135)
    cadetblue	          = 0x5F9EA0,     // RGB(95,158,160)
    chartreuse	          = 0x7FFF00,     // RGB(127,255,0)
    chocolate	          = 0xD2691E,     // RGB(210,105,30)
    coral	              = 0xFF7F50,     // RGB(255,127,80)
    cornflowerblue	      = 0x6495ED,     // RGB(100,149,237)
    cornsilk	          = 0xFFF8DC,     // RGB(255,248,220)
    crimson	              = 0xDC143C,     // RGB(220,20,60)
    cyan	              = 0x00FFFF,     // RGB(0,255,255)
    darkblue	          = 0x00008B,     // RGB(0,0,139)
    darkcyan	          = 0x008B8B,     // RGB(0,139,139)
    darkgoldenrod	      = 0xB8860B,     // RGB(184,134,11)
    darkgray	          = 0xA9A9A9,     // RGB(169,169,169)
    darkgreen	          = 0x006400,     // RGB(0,100,0)
    darkgrey	          = 0xA9A9A9,     // RGB(169,169,169)
    darkkhaki	          = 0xBDB76B,     // RGB(189,183,107)
    darkmagenta	          = 0x8B008B,     // RGB(139,0,139)
    darkolivegreen	      = 0x556B2F,     // RGB(85,107,47)
    darkorange	          = 0xFF8C00,     // RGB(255,140,0)
    darkorchid	          = 0x9932CC,     // RGB(153,50,204)
    darkred	              = 0x8B0000,     // RGB(139,0,0)
    darksalmon	          = 0xE9967A,     // RGB(233,150,122)
    darkseagreen	      = 0x8FBC8F,     // RGB(143,188,143)
    darkslateblue	      = 0x483D8B,     // RGB(72,61,139)
    darkslategray	      = 0x2F4F4F,     // RGB(47,79,79)
    darkslategrey	      = 0x2F4F4F,     // RGB(47,79,79)
    darkturquoise	      = 0x00CED1,     // RGB(0,206,209)
    darkviolet	          = 0x9400D3,     // RGB(148,0,211)
    deeppink	          = 0xFF1493,     // RGB(255,20,147)
    deepskyblue	          = 0x00BFFF,     // RGB(0,191,255)
    dimgray	              = 0x696969,     // RGB(105,105,105)
    dimgrey	              = 0x696969,     // RGB(105,105,105)
    dodgerblue	          = 0x1E90FF,     // RGB(30,144,255)
    firebrick	          = 0xB22222,     // RGB(178,34,34)
    floralwhite	          = 0xFFFAF0,     // RGB(255,250,240)
    forestgreen	          = 0x228B22,     // RGB(34,139,34)
    fuchsia	              = 0xFF00FF,     // RGB(255,0,255)
    gainsboro	          = 0xDCDCDC,     // RGB(220,220,220)
    ghostwhite	          = 0xF8F8FF,     // RGB(248,248,255)
    gold	              = 0xFFD700,     // RGB(255,215,0)
    goldenrod	          = 0xDAA520,     // RGB(218,165,32)
    gray	              = 0x808080,     // RGB(128,128,128)
    green	              = 0x008000,     // RGB(0,128,0)
    greenyellow	          = 0xADFF2F,     // RGB(173,255,47)
    grey	              = 0x808080,     // RGB(128,128,128)
    honeydew	          = 0xF0FFF0,     // RGB(240,255,240)
    hotpink	              = 0xFF69B4,     // RGB(255,105,180)
    indianred	          = 0xCD5C5C,     // RGB(205,92,92)
    indigo	              = 0x4B0082,     // RGB(75,0,130)
    ivory	              = 0xFFFFF0,     // RGB(255,255,240)
    khaki	              = 0xF0E68C,     // RGB(240,230,140)
    lavender	          = 0xE6E6FA,     // RGB(230,230,250)
    lavenderblush	      = 0xFFF0F5,     // RGB(255,240,245)
    lawngreen	          = 0x7CFC00,     // RGB(124,252,0)
    lemonchiffon	      = 0xFFFACD,     // RGB(255,250,205)
    lightblue	          = 0xADD8E6,     // RGB(173,216,230)
    lightcoral	          = 0xF08080,     // RGB(240,128,128)
    lightcyan	          = 0xE0FFFF,     // RGB(224,255,255)
    lightgoldenrodyellow  = 0xFAFAD2,     // RGB(250,250,210)
    lightgray	          = 0xD3D3D3,     // RGB(211,211,211)
    lightgreen	          = 0x90EE90,     // RGB(144,238,144)
    lightgrey	          = 0xD3D3D3,     // RGB(211,211,211)
    lightpink	          = 0xFFB6C1,     // RGB(255,182,193)
    lightsalmon	          = 0xFFA07A,     // RGB(255,160,122)
    lightseagreen	      = 0x20B2AA,     // RGB(32,178,170)
    lightskyblue	      = 0x87CEFA,     // RGB(135,206,250)
    lightslategray	      = 0x778899,     // RGB(119,136,153)
    lightslategrey	      = 0x778899,     // RGB(119,136,153)
    lightsteelblue	      = 0xB0C4DE,     // RGB(176,196,222)
    lightyellow	          = 0xFFFFE0,     // RGB(255,255,224)
    lime	              = 0x00FF00,     // RGB(0,255,0)
    limegreen	          = 0x32CD32,     // RGB(50,205,50)
    linen	              = 0xFAF0E6,     // RGB(250,240,230)
    magenta	              = 0xFF00FF,     // RGB(255,0,255)
    maroon	              = 0x800000,     // RGB(128,0,0)
    mediumaquamarine	  = 0x66CDAA,     // RGB(102,205,170)
    mediumblue	          = 0x0000CD,     // RGB(0,0,205)
    mediumorchid	      = 0xBA55D3,     // RGB(186,85,211)
    mediumpurple	      = 0x9370DB,     // RGB(147,112,219)
    mediumseagreen	      = 0x3CB371,     // RGB(60,179,113)
    mediumslateblue	      = 0x7B68EE,     // RGB(123,104,238)
    mediumspringgreen	  = 0x00FA9A,     // RGB(0,250,154)
    mediumturquoise	      = 0x48D1CC,     // RGB(72,209,204)
    mediumvioletred	      = 0xC71585,     // RGB(199,21,133)
    midnightblue	      = 0x191970,     // RGB(25,25,112)
    mintcream	          = 0xF5FFFA,     // RGB(245,255,250)
    mistyrose	          = 0xFFE4E1,     // RGB(255,228,225)
    moccasin	          = 0xFFE4B5,     // RGB(255,228,181)
    navajowhite	          = 0xFFDEAD,     // RGB(255,222,173)
    navy	              = 0x000080,     // RGB(0,0,128)
    oldlace	              = 0xFDF5E6,     // RGB(253,245,230)
    olive	              = 0x808000,     // RGB(128,128,0)
    olivedrab	          = 0x6B8E23,     // RGB(107,142,35)
    orange	              = 0xFFA500,     // RGB(255,165,0)
    orangered	          = 0xFF4500,     // RGB(255,69,0)
    orchid	              = 0xDA70D6,     // RGB(218,112,214)
    palegoldenrod	      = 0xEEE8AA,     // RGB(238,232,170)
    palegreen	          = 0x98FB98,     // RGB(152,251,152)
    paleturquoise	      = 0xAFEEEE,     // RGB(175,238,238)
    palevioletred	      = 0xDB7093,     // RGB(219,112,147)
    papayawhip	          = 0xFFEFD5,     // RGB(255,239,213)
    peachpuff	          = 0xFFDAB9,     // RGB(255,218,185)
    peru	              = 0xCD853F,     // RGB(205,133,63)
    pink	              = 0xFFC0CB,     // RGB(255,192,203)
    plum	              = 0xDDA0DD,     // RGB(221,160,221)
    powderblue	          = 0xB0E0E6,     // RGB(176,224,230)
    purple	              = 0x800080,     // RGB(128,0,128)
    red	                  = 0xFF0000,     // RGB(255,0,0)
    rosybrown	          = 0xBC8F8F,     // RGB(188,143,143)
    royalblue	          = 0x4169E1,     // RGB(65,105,225)
    saddlebrown	          = 0x8B4513,     // RGB(139,69,19)
    salmon	              = 0xFA8072,     // RGB(250,128,114)
    sandybrown	          = 0xF4A460,     // RGB(244,164,96)
    seagreen	          = 0x2E8B57,     // RGB(46,139,87)
    seashell	          = 0xFFF5EE,     // RGB(255,245,238)
    sienna	              = 0xA0522D,     // RGB(160,82,45)
    silver	              = 0xC0C0C0,     // RGB(192,192,192)
    skyblue	              = 0x87CEEB,     // RGB(135,206,235)
    slateblue	          = 0x6A5ACD,     // RGB(106,90,205)
    slategray	          = 0x708090,     // RGB(112,128,144)
    slategrey	          = 0x708090,     // RGB(112,128,144)
    snow	              = 0xFFFAFA,     // RGB(255,250,250)
    springgreen	          = 0x00FF7F,     // RGB(0,255,127)
    steelblue	          = 0x4682B4,     // RGB(70,130,180)
    tan	                  = 0xD2B48C,     // RGB(210,180,140)
    teal	              = 0x008080,     // RGB(0,128,128)
    thistle	              = 0xD8BFD8,     // RGB(216,191,216)
    tomato	              = 0xFF6347,     // RGB(255,99,71)
    turquoise	          = 0x40E0D0,     // RGB(64,224,208)
    violet	              = 0xEE82EE,     // RGB(238,130,238)
    wheat	              = 0xF5DEB3,     // RGB(245,222,179)
    white	              = 0xFFFFFF,     // RGB(255,255,255)
    whitesmoke	          = 0xF5F5F5,     // RGB(245,245,245)
    yellow	              = 0xFFFF00,     // RGB(255,255,0)
    yellowgreen	          = 0x9ACD32,     // RGB(154,205,50)

};  // enum class color

/// Color names from the X11 source code
/// Source: https://gitlab.freedesktop.org/xorg/app/rgb/raw/master/rgb.txt
enum class x11_color : std::uint32_t {
    snow = 0xFFFAFA,                              //rgb(255,250,250)
    ghost_white = 0xF8F8FF,                       //rgb(248,248,255)
    white_smoke = 0xF5F5F5,                       //rgb(245,245,245)
    gainsboro = 0xDCDCDC,                         //rgb(220,220,220)
    floral_white = 0xFFFAF0,                      //rgb(255,250,240)
    old_lace = 0xFDF5E6,                          //rgb(253,245,230)
    linen = 0xFAF0E6,                             //rgb(250,240,230)
    antique_white = 0xFAEBD7,                     //rgb(250,235,215)
    papaya_whip = 0xFFEFD5,                       //rgb(255,239,213)
    blanched_almond = 0xFFEBCD,                   //rgb(255,235,205)
    bisque = 0xFFE4C4,                            //rgb(255,228,196)
    peach_puff = 0xFFDAB9,                        //rgb(255,218,185)
    navajo_white = 0xFFDEAD,                      //rgb(255,222,173)
    moccasin = 0xFFE4B5,                          //rgb(255,228,181)
    cornsilk = 0xFFF8DC,                          //rgb(255,248,220)
    ivory = 0xFFFFF0,                             //rgb(255,255,240)
    lemon_chiffon = 0xFFFACD,                     //rgb(255,250,205)
    seashell = 0xFFF5EE,                          //rgb(255,245,238)
    honeydew = 0xF0FFF0,                          //rgb(240,255,240)
    mint_cream = 0xF5FFFA,                        //rgb(245,255,250)
    azure = 0xF0FFFF,                             //rgb(240,255,255)
    alice_blue = 0xF0F8FF,                        //rgb(240,248,255)
    lavender = 0xE6E6FA,                          //rgb(230,230,250)
    lavender_blush = 0xFFF0F5,                    //rgb(255,240,245)
    misty_rose = 0xFFE4E1,                        //rgb(255,228,225)
    white = 0xFFFFFF,                             //rgb(255,255,255)
    dim_gray = 0x696969,                          //rgb(105,105,105)
    dim_grey = 0x696969,                          //rgb(105,105,105)
    slate_gray = 0x708090,                        //rgb(112,128,144)
    slate_grey = 0x708090,                        //rgb(112,128,144)
    light_slate_gray = 0x778899,                  //rgb(119,136,153)
    light_slate_grey = 0x778899,                  //rgb(119,136,153)
    gray = 0xBEBEBE,                              //rgb(190,190,190)
    grey = 0xBEBEBE,                              //rgb(190,190,190)
    x11_gray = 0xBEBEBE,                          //rgb(190,190,190)
    x11_grey = 0xBEBEBE,                          //rgb(190,190,190)
    web_gray = 0x808080,                          //rgb(128,128,128)
    web_grey = 0x808080,                          //rgb(128,128,128)
    light_grey = 0xD3D3D3,                        //rgb(211,211,211)
    light_gray = 0xD3D3D3,                        //rgb(211,211,211)
    cornflower_blue = 0x6495ED,                   //rgb(100,149,237)
    slate_blue = 0x6A5ACD,                        //rgb(106,90,205)
    medium_slate_blue = 0x7B68EE,                 //rgb(123,104,238)
    light_slate_blue = 0x8470FF,                  //rgb(132,112,255)
    sky_blue = 0x87CEEB,                          //rgb(135,206,235)
    light_sky_blue = 0x87CEFA,                    //rgb(135,206,250)
    light_steel_blue = 0xB0C4DE,                  //rgb(176,196,222)
    light_blue = 0xADD8E6,                        //rgb(173,216,230)
    powder_blue = 0xB0E0E6,                       //rgb(176,224,230)
    pale_turquoise = 0xAFEEEE,                    //rgb(175,238,238)
    light_cyan = 0xE0FFFF,                        //rgb(224,255,255)
    medium_aquamarine = 0x66CDAA,                 //rgb(102,205,170)
    aquamarine = 0x7FFFD4,                        //rgb(127,255,212)
    dark_sea_green = 0x8FBC8F,                    //rgb(143,188,143)
    pale_green = 0x98FB98,                        //rgb(152,251,152)
    lawn_green = 0x7CFC00,                        //rgb(124,252,0)
    chartreuse = 0x7FFF00,                        //rgb(127,255,0)
    green_yellow = 0xADFF2F,                      //rgb(173,255,47)
    yellow_green = 0x9ACD32,                      //rgb(154,205,50)
    olive_drab = 0x6B8E23,                        //rgb(107,142,35)
    dark_khaki = 0xBDB76B,                        //rgb(189,183,107)
    khaki = 0xF0E68C,                             //rgb(240,230,140)
    pale_goldenrod = 0xEEE8AA,                    //rgb(238,232,170)
    light_goldenrod_yellow = 0xFAFAD2,            //rgb(250,250,210)
    light_yellow = 0xFFFFE0,                      //rgb(255,255,224)
    yellow = 0xFFFF00,                            //rgb(255,255,0)
    gold = 0xFFD700,                              //rgb(255,215,0)
    light_goldenrod = 0xEEDD82,                   //rgb(238,221,130)
    goldenrod = 0xDAA520,                         //rgb(218,165,32)
    dark_goldenrod = 0xB8860B,                    //rgb(184,134,11)
    rosy_brown = 0xBC8F8F,                        //rgb(188,143,143)
    indian_red = 0xCD5C5C,                        //rgb(205,92,92)
    saddle_brown = 0x8B4513,                      //rgb(139,69,19)
    sienna = 0xA0522D,                            //rgb(160,82,45)
    peru = 0xCD853F,                              //rgb(205,133,63)
    burlywood = 0xDEB887,                         //rgb(222,184,135)
    beige = 0xF5F5DC,                             //rgb(245,245,220)
    wheat = 0xF5DEB3,                             //rgb(245,222,179)
    sandy_brown = 0xF4A460,                       //rgb(244,164,96)
    tan = 0xD2B48C,                               //rgb(210,180,140)
    chocolate = 0xD2691E,                         //rgb(210,105,30)
    firebrick = 0xB22222,                         //rgb(178,34,34)
    brown = 0xA52A2A,                             //rgb(165,42,42)
    dark_salmon = 0xE9967A,                       //rgb(233,150,122)
    salmon = 0xFA8072,                            //rgb(250,128,114)
    light_salmon = 0xFFA07A,                      //rgb(255,160,122)
    orange = 0xFFA500,                            //rgb(255,165,0)
    dark_orange = 0xFF8C00,                       //rgb(255,140,0)
    coral = 0xFF7F50,                             //rgb(255,127,80)
    light_coral = 0xF08080,                       //rgb(240,128,128)
    tomato = 0xFF6347,                            //rgb(255,99,71)
    orange_red = 0xFF4500,                        //rgb(255,69,0)
    red = 0xFF0000,                               //rgb(255,0,0)
    hot_pink = 0xFF69B4,                          //rgb(255,105,180)
    deep_pink = 0xFF1493,                         //rgb(255,20,147)
    pink = 0xFFC0CB,                              //rgb(255,192,203)
    light_pink = 0xFFB6C1,                        //rgb(255,182,193)
    pale_violet_red = 0xDB7093,                   //rgb(219,112,147)
    maroon = 0xB03060,                            //rgb(176,48,96)
    x11_maroon = 0xB03060,                        //rgb(176,48,96)
    web_maroon = 0x800000,                        //rgb(128,0,0)
    medium_violet_red = 0xC71585,                 //rgb(199,21,133)
    violet_red = 0xD02090,                        //rgb(208,32,144)
    magenta = 0xFF00FF,                           //rgb(255,0,255)
    fuchsia = 0xFF00FF,                           //rgb(255,0,255)
    violet = 0xEE82EE,                            //rgb(238,130,238)
    plum = 0xDDA0DD,                              //rgb(221,160,221)
    orchid = 0xDA70D6,                            //rgb(218,112,214)
    medium_orchid = 0xBA55D3,                     //rgb(186,85,211)
    dark_orchid = 0x9932CC,                       //rgb(153,50,204)
    dark_violet = 0x9400D3,                       //rgb(148,0,211)
    blue_violet = 0x8A2BE2,                       //rgb(138,43,226)
    purple = 0xA020F0,                            //rgb(160,32,240)
    x11_purple = 0xA020F0,                        //rgb(160,32,240)
    web_purple = 0x800080,                        //rgb(128,0,128)
    medium_purple = 0x9370DB,                     //rgb(147,112,219)
    thistle = 0xD8BFD8,                           //rgb(216,191,216)
    snow1 = 0xFFFAFA,                             //rgb(255,250,250)
    snow2 = 0xEEE9E9,                             //rgb(238,233,233)
    snow3 = 0xCDC9C9,                             //rgb(205,201,201)
    snow4 = 0x8B8989,                             //rgb(139,137,137)
    seashell1 = 0xFFF5EE,                         //rgb(255,245,238)
    seashell2 = 0xEEE5DE,                         //rgb(238,229,222)
    seashell3 = 0xCDC5BF,                         //rgb(205,197,191)
    seashell4 = 0x8B8682,                         //rgb(139,134,130)
    bisque1 = 0xFFE4C4,                           //rgb(255,228,196)
    bisque2 = 0xEED5B7,                           //rgb(238,213,183)
    bisque3 = 0xCDB79E,                           //rgb(205,183,158)
    bisque4 = 0x8B7D6B,                           //rgb(139,125,107)
    cornsilk1 = 0xFFF8DC,                         //rgb(255,248,220)
    cornsilk2 = 0xEEE8CD,                         //rgb(238,232,205)
    cornsilk3 = 0xCDC8B1,                         //rgb(205,200,177)
    cornsilk4 = 0x8B8878,                         //rgb(139,136,120)
    ivory1 = 0xFFFFF0,                            //rgb(255,255,240)
    ivory2 = 0xEEEEE0,                            //rgb(238,238,224)
    ivory3 = 0xCDCDC1,                            //rgb(205,205,193)
    ivory4 = 0x8B8B83,                            //rgb(139,139,131)
    honeydew1 = 0xF0FFF0,                         //rgb(240,255,240)
    honeydew2 = 0xE0EEE0,                         //rgb(224,238,224)
    honeydew3 = 0xC1CDC1,                         //rgb(193,205,193)
    honeydew4 = 0x838B83,                         //rgb(131,139,131)
    azure1 = 0xF0FFFF,                            //rgb(240,255,255)
    azure2 = 0xE0EEEE,                            //rgb(224,238,238)
    azure3 = 0xC1CDCD,                            //rgb(193,205,205)
    azure4 = 0x838B8B,                            //rgb(131,139,139)
    aquamarine1 = 0x7FFFD4,                       //rgb(127,255,212)
    aquamarine2 = 0x76EEC6,                       //rgb(118,238,198)
    aquamarine3 = 0x66CDAA,                       //rgb(102,205,170)
    chartreuse1 = 0x7FFF00,                       //rgb(127,255,0)
    chartreuse2 = 0x76EE00,                       //rgb(118,238,0)
    chartreuse3 = 0x66CD00,                       //rgb(102,205,0)
    khaki1 = 0xFFF68F,                            //rgb(255,246,143)
    khaki2 = 0xEEE685,                            //rgb(238,230,133)
    khaki3 = 0xCDC673,                            //rgb(205,198,115)
    khaki4 = 0x8B864E,                            //rgb(139,134,78)
    yellow1 = 0xFFFF00,                           //rgb(255,255,0)
    yellow2 = 0xEEEE00,                           //rgb(238,238,0)
    yellow3 = 0xCDCD00,                           //rgb(205,205,0)
    yellow4 = 0x8B8B00,                           //rgb(139,139,0)
    gold1 = 0xFFD700,                             //rgb(255,215,0)
    gold2 = 0xEEC900,                             //rgb(238,201,0)
    gold3 = 0xCDAD00,                             //rgb(205,173,0)
    gold4 = 0x8B7500,                             //rgb(139,117,0)
    goldenrod1 = 0xFFC125,                        //rgb(255,193,37)
    goldenrod2 = 0xEEB422,                        //rgb(238,180,34)
    goldenrod3 = 0xCD9B1D,                        //rgb(205,155,29)
    goldenrod4 = 0x8B6914,                        //rgb(139,105,20)
    sienna1 = 0xFF8247,                           //rgb(255,130,71)
    sienna2 = 0xEE7942,                           //rgb(238,121,66)
    sienna3 = 0xCD6839,                           //rgb(205,104,57)
    sienna4 = 0x8B4726,                           //rgb(139,71,38)
    burlywood1 = 0xFFD39B,                        //rgb(255,211,155)
    burlywood2 = 0xEEC591,                        //rgb(238,197,145)
    burlywood3 = 0xCDAA7D,                        //rgb(205,170,125)
    burlywood4 = 0x8B7355,                        //rgb(139,115,85)
    wheat1 = 0xFFE7BA,                            //rgb(255,231,186)
    wheat2 = 0xEED8AE,                            //rgb(238,216,174)
    wheat3 = 0xCDBA96,                            //rgb(205,186,150)
    wheat4 = 0x8B7E66,                            //rgb(139,126,102)
    tan1 = 0xFFA54F,                              //rgb(255,165,79)
    tan2 = 0xEE9A49,                              //rgb(238,154,73)
    tan3 = 0xCD853F,                              //rgb(205,133,63)
    tan4 = 0x8B5A2B,                              //rgb(139,90,43)
    chocolate1 = 0xFF7F24,                        //rgb(255,127,36)
    chocolate2 = 0xEE7621,                        //rgb(238,118,33)
    chocolate3 = 0xCD661D,                        //rgb(205,102,29)
    chocolate4 = 0x8B4513,                        //rgb(139,69,19)
    firebrick1 = 0xFF3030,                        //rgb(255,48,48)
    firebrick2 = 0xEE2C2C,                        //rgb(238,44,44)
    firebrick3 = 0xCD2626,                        //rgb(205,38,38)
    firebrick4 = 0x8B1A1A,                        //rgb(139,26,26)
    brown1 = 0xFF4040,                            //rgb(255,64,64)
    brown2 = 0xEE3B3B,                            //rgb(238,59,59)
    brown3 = 0xCD3333,                            //rgb(205,51,51)
    brown4 = 0x8B2323,                            //rgb(139,35,35)
    salmon1 = 0xFF8C69,                           //rgb(255,140,105)
    salmon2 = 0xEE8262,                           //rgb(238,130,98)
    salmon3 = 0xCD7054,                           //rgb(205,112,84)
    salmon4 = 0x8B4C39,                           //rgb(139,76,57)
    orange1 = 0xFFA500,                           //rgb(255,165,0)
    orange2 = 0xEE9A00,                           //rgb(238,154,0)
    orange3 = 0xCD8500,                           //rgb(205,133,0)
    orange4 = 0x8B5A00,                           //rgb(139,90,0)
    coral1 = 0xFF7256,                            //rgb(255,114,86)
    coral2 = 0xEE6A50,                            //rgb(238,106,80)
    coral3 = 0xCD5B45,                            //rgb(205,91,69)
    coral4 = 0x8B3E2F,                            //rgb(139,62,47)
    tomato1 = 0xFF6347,                           //rgb(255,99,71)
    tomato2 = 0xEE5C42,                           //rgb(238,92,66)
    tomato3 = 0xCD4F39,                           //rgb(205,79,57)
    tomato4 = 0x8B3626,                           //rgb(139,54,38)
    red1 = 0xFF0000,                              //rgb(255,0,0)
    red2 = 0xEE0000,                              //rgb(238,0,0)
    red3 = 0xCD0000,                              //rgb(205,0,0)
    red4 = 0x8B0000,                              //rgb(139,0,0)
    pink1 = 0xFFB5C5,                             //rgb(255,181,197)
    pink2 = 0xEEA9B8,                             //rgb(238,169,184)
    pink3 = 0xCD919E,                             //rgb(205,145,158)
    pink4 = 0x8B636C,                             //rgb(139,99,108)
    maroon1 = 0xFF34B3,                           //rgb(255,52,179)
    maroon2 = 0xEE30A7,                           //rgb(238,48,167)
    maroon3 = 0xCD2990,                           //rgb(205,41,144)
    maroon4 = 0x8B1C62,                           //rgb(139,28,98)
    magenta1 = 0xFF00FF,                          //rgb(255,0,255)
    magenta2 = 0xEE00EE,                          //rgb(238,0,238)
    magenta3 = 0xCD00CD,                          //rgb(205,0,205)
    magenta4 = 0x8B008B,                          //rgb(139,0,139)
    orchid1 = 0xFF83FA,                           //rgb(255,131,250)
    orchid2 = 0xEE7AE9,                           //rgb(238,122,233)
    orchid3 = 0xCD69C9,                           //rgb(205,105,201)
    orchid4 = 0x8B4789,                           //rgb(139,71,137)
    plum1 = 0xFFBBFF,                             //rgb(255,187,255)
    plum2 = 0xEEAEEE,                             //rgb(238,174,238)
    plum3 = 0xCD96CD,                             //rgb(205,150,205)
    plum4 = 0x8B668B,                             //rgb(139,102,139)
    purple1 = 0x9B30FF,                           //rgb(155,48,255)
    purple2 = 0x912CEE,                           //rgb(145,44,238)
    purple3 = 0x7D26CD,                           //rgb(125,38,205)
    thistle1 = 0xFFE1FF,                          //rgb(255,225,255)
    thistle2 = 0xEED2EE,                          //rgb(238,210,238)
    thistle3 = 0xCDB5CD,                          //rgb(205,181,205)
    thistle4 = 0x8B7B8B,                          //rgb(139,123,139)
    gray40 = 0x666666,                            //rgb(102,102,102)
    grey40 = 0x666666,                            //rgb(102,102,102)
    gray41 = 0x696969,                            //rgb(105,105,105)
    grey41 = 0x696969,                            //rgb(105,105,105)
    gray42 = 0x6B6B6B,                            //rgb(107,107,107)
    grey42 = 0x6B6B6B,                            //rgb(107,107,107)
    gray43 = 0x6E6E6E,                            //rgb(110,110,110)
    grey43 = 0x6E6E6E,                            //rgb(110,110,110)
    gray44 = 0x707070,                            //rgb(112,112,112)
    grey44 = 0x707070,                            //rgb(112,112,112)
    gray45 = 0x737373,                            //rgb(115,115,115)
    grey45 = 0x737373,                            //rgb(115,115,115)
    gray46 = 0x757575,                            //rgb(117,117,117)
    grey46 = 0x757575,                            //rgb(117,117,117)
    gray47 = 0x787878,                            //rgb(120,120,120)
    grey47 = 0x787878,                            //rgb(120,120,120)
    gray48 = 0x7A7A7A,                            //rgb(122,122,122)
    grey48 = 0x7A7A7A,                            //rgb(122,122,122)
    gray49 = 0x7D7D7D,                            //rgb(125,125,125)
    grey49 = 0x7D7D7D,                            //rgb(125,125,125)
    gray50 = 0x7F7F7F,                            //rgb(127,127,127)
    grey50 = 0x7F7F7F,                            //rgb(127,127,127)
    gray51 = 0x828282,                            //rgb(130,130,130)
    grey51 = 0x828282,                            //rgb(130,130,130)
    gray52 = 0x858585,                            //rgb(133,133,133)
    grey52 = 0x858585,                            //rgb(133,133,133)
    gray53 = 0x878787,                            //rgb(135,135,135)
    grey53 = 0x878787,                            //rgb(135,135,135)
    gray54 = 0x8A8A8A,                            //rgb(138,138,138)
    grey54 = 0x8A8A8A,                            //rgb(138,138,138)
    gray55 = 0x8C8C8C,                            //rgb(140,140,140)
    grey55 = 0x8C8C8C,                            //rgb(140,140,140)
    gray56 = 0x8F8F8F,                            //rgb(143,143,143)
    grey56 = 0x8F8F8F,                            //rgb(143,143,143)
    gray57 = 0x919191,                            //rgb(145,145,145)
    grey57 = 0x919191,                            //rgb(145,145,145)
    gray58 = 0x949494,                            //rgb(148,148,148)
    grey58 = 0x949494,                            //rgb(148,148,148)
    gray59 = 0x969696,                            //rgb(150,150,150)
    grey59 = 0x969696,                            //rgb(150,150,150)
    gray60 = 0x999999,                            //rgb(153,153,153)
    grey60 = 0x999999,                            //rgb(153,153,153)
    gray61 = 0x9C9C9C,                            //rgb(156,156,156)
    grey61 = 0x9C9C9C,                            //rgb(156,156,156)
    gray62 = 0x9E9E9E,                            //rgb(158,158,158)
    grey62 = 0x9E9E9E,                            //rgb(158,158,158)
    gray63 = 0xA1A1A1,                            //rgb(161,161,161)
    grey63 = 0xA1A1A1,                            //rgb(161,161,161)
    gray64 = 0xA3A3A3,                            //rgb(163,163,163)
    grey64 = 0xA3A3A3,                            //rgb(163,163,163)
    gray65 = 0xA6A6A6,                            //rgb(166,166,166)
    grey65 = 0xA6A6A6,                            //rgb(166,166,166)
    gray66 = 0xA8A8A8,                            //rgb(168,168,168)
    grey66 = 0xA8A8A8,                            //rgb(168,168,168)
    gray67 = 0xABABAB,                            //rgb(171,171,171)
    grey67 = 0xABABAB,                            //rgb(171,171,171)
    gray68 = 0xADADAD,                            //rgb(173,173,173)
    grey68 = 0xADADAD,                            //rgb(173,173,173)
    gray69 = 0xB0B0B0,                            //rgb(176,176,176)
    grey69 = 0xB0B0B0,                            //rgb(176,176,176)
    gray70 = 0xB3B3B3,                            //rgb(179,179,179)
    grey70 = 0xB3B3B3,                            //rgb(179,179,179)
    gray71 = 0xB5B5B5,                            //rgb(181,181,181)
    grey71 = 0xB5B5B5,                            //rgb(181,181,181)
    gray72 = 0xB8B8B8,                            //rgb(184,184,184)
    grey72 = 0xB8B8B8,                            //rgb(184,184,184)
    gray73 = 0xBABABA,                            //rgb(186,186,186)
    grey73 = 0xBABABA,                            //rgb(186,186,186)
    gray74 = 0xBDBDBD,                            //rgb(189,189,189)
    grey74 = 0xBDBDBD,                            //rgb(189,189,189)
    gray75 = 0xBFBFBF,                            //rgb(191,191,191)
    grey75 = 0xBFBFBF,                            //rgb(191,191,191)
    gray76 = 0xC2C2C2,                            //rgb(194,194,194)
    grey76 = 0xC2C2C2,                            //rgb(194,194,194)
    gray77 = 0xC4C4C4,                            //rgb(196,196,196)
    grey77 = 0xC4C4C4,                            //rgb(196,196,196)
    gray78 = 0xC7C7C7,                            //rgb(199,199,199)
    grey78 = 0xC7C7C7,                            //rgb(199,199,199)
    gray79 = 0xC9C9C9,                            //rgb(201,201,201)
    grey79 = 0xC9C9C9,                            //rgb(201,201,201)
    gray80 = 0xCCCCCC,                            //rgb(204,204,204)
    grey80 = 0xCCCCCC,                            //rgb(204,204,204)
    gray81 = 0xCFCFCF,                            //rgb(207,207,207)
    grey81 = 0xCFCFCF,                            //rgb(207,207,207)
    gray82 = 0xD1D1D1,                            //rgb(209,209,209)
    grey82 = 0xD1D1D1,                            //rgb(209,209,209)
    gray83 = 0xD4D4D4,                            //rgb(212,212,212)
    grey83 = 0xD4D4D4,                            //rgb(212,212,212)
    gray84 = 0xD6D6D6,                            //rgb(214,214,214)
    grey84 = 0xD6D6D6,                            //rgb(214,214,214)
    gray85 = 0xD9D9D9,                            //rgb(217,217,217)
    grey85 = 0xD9D9D9,                            //rgb(217,217,217)
    gray86 = 0xDBDBDB,                            //rgb(219,219,219)
    grey86 = 0xDBDBDB,                            //rgb(219,219,219)
    gray87 = 0xDEDEDE,                            //rgb(222,222,222)
    grey87 = 0xDEDEDE,                            //rgb(222,222,222)
    gray88 = 0xE0E0E0,                            //rgb(224,224,224)
    grey88 = 0xE0E0E0,                            //rgb(224,224,224)
    gray89 = 0xE3E3E3,                            //rgb(227,227,227)
    grey89 = 0xE3E3E3,                            //rgb(227,227,227)
    gray90 = 0xE5E5E5,                            //rgb(229,229,229)
    grey90 = 0xE5E5E5,                            //rgb(229,229,229)
    gray91 = 0xE8E8E8,                            //rgb(232,232,232)
    grey91 = 0xE8E8E8,                            //rgb(232,232,232)
    gray92 = 0xEBEBEB,                            //rgb(235,235,235)
    grey92 = 0xEBEBEB,                            //rgb(235,235,235)
    gray93 = 0xEDEDED,                            //rgb(237,237,237)
    grey93 = 0xEDEDED,                            //rgb(237,237,237)
    gray94 = 0xF0F0F0,                            //rgb(240,240,240)
    grey94 = 0xF0F0F0,                            //rgb(240,240,240)
    gray95 = 0xF2F2F2,                            //rgb(242,242,242)
    grey95 = 0xF2F2F2,                            //rgb(242,242,242)
    gray96 = 0xF5F5F5,                            //rgb(245,245,245)
    grey96 = 0xF5F5F5,                            //rgb(245,245,245)
    gray97 = 0xF7F7F7,                            //rgb(247,247,247)
    grey97 = 0xF7F7F7,                            //rgb(247,247,247)
    gray98 = 0xFAFAFA,                            //rgb(250,250,250)
    grey98 = 0xFAFAFA,                            //rgb(250,250,250)
    gray99 = 0xFCFCFC,                            //rgb(252,252,252)
    grey99 = 0xFCFCFC,                            //rgb(252,252,252)
    gray100 = 0xFFFFFF,                           //rgb(255,255,255)
    grey100 = 0xFFFFFF,                           //rgb(255,255,255)
    dark_grey = 0xA9A9A9,                         //rgb(169,169,169)
    dark_gray = 0xA9A9A9,                         //rgb(169,169,169)
    dark_blue = 0x00008B,                         //rgb(0,0,139)
    dark_cyan = 0x008B8B,                         //rgb(0,139,139)
    dark_magenta = 0x8B008B,                      //rgb(139,0,139)
    dark_red = 0x8B0000,                          //rgb(139,0,0)
    light_green = 0x90EE90,                       //rgb(144,238,144)
    crimson = 0xDC143C,                           //rgb(220,20,60)
    olive = 0x808000,                             //rgb(128,128,0)
    rebecca_purple = 0x663399,                    //rgb(102,51,153)
    silver = 0xC0C0C0,                            //rgb(192,192,192)
};  // enum class color

/// Color names as defined by the Printer Working Group (PWG) of IEEE in PWG 5101.1
/// Source: https://en.wikipedia.org/wiki/X11_color_names#Derived_lists
enum class pwc_color : std::uint32_t
{
    black	        = 0x000000,
    light_black	    = 0x808080,
    blue	        = 0x0000FF,
    dark_blue	    = 0x00008B,
    light_blue	    = 0xADD8E6,
    brown	        = 0xA52A2A,
    dark_brown	    = 0x5C4033,
    light_brown	    = 0x9966FF,
    buff	        = 0xF0DC82,
    dark_buff	    = 0x976638,
    light_buff	    = 0xECD9B0,
    cyan	        = 0x00FFFF,
    dark_cyan	    = 0x008B8B,
    light_cyan	    = 0xE0FFFF,
    gold	        = 0xFFD700,
    dark_gold	    = 0xEEBC1D,
    light_gold	    = 0xF1E5AC,
    goldenrod	    = 0xDAA520,
    dark_goldenrod	= 0xB8860B,
    light_goldenrod	= 0xFFEC8B,
    gray	        = 0x808080,
    dark_gray	    = 0x404040,
    light_gray	    = 0xD3D3D3,
    green	        = 0x008000,
    dark_green	    = 0x006400,
    light_green	    = 0x90EE90,
    ivory	        = 0xFFFFF0,
    dark_ivory	    = 0xF2E58F,
    light_ivory	    = 0xFFF8C9,
    magenta	        = 0xFF00FF,
    dark_magenta	= 0x8B008B,
    light_magenta	= 0xFF77FF,
    mustard	        = 0xFFDB58,
    dark_mustard	= 0x7C7C40,
    light_mustard	= 0xEEDD62,
    orange	        = 0xFFA500,
    dark_orange	    = 0xFF8C00,
    light_orange	= 0xD9A465,
    pink	        = 0xFFC0CB,
    dark_pink	    = 0xE75480,
    light_pink	    = 0xFFB6C1,
    red	            = 0xFF0000,
    dark_red	    = 0x8B0000,
    light_red	    = 0xFF3333,
    silver	        = 0xC0C0C0,
    dark_silver	    = 0xAFAFAF,
    light_silver	= 0xE1E1E1,
    turquoise	    = 0x30D5C8,
    dark_turquoise	= 0x00CED1,
    light_turquoise	= 0xAFE4DE,
    violet	        = 0xEE82EE,
    dark_violet	    = 0x9400D3,
    light_violet	= 0x7A5299,
    white	        = 0xFFFFFF,
    yellow	        = 0xFFFF00,
    dark_yellow     = 0xFFCC00,
    light_yellow	= 0xFFFFE0,
};

TERMCONTROL_DEFINE_COLOR_ENUM(css_color);
TERMCONTROL_DEFINE_COLOR_ENUM(x11_color);
TERMCONTROL_DEFINE_COLOR_ENUM(pwc_color);

/// 24 bit color
struct rgb_color {
    constexpr rgb_color() noexcept
            : r(0), g(0), b(0) { }

    constexpr rgb_color(std::uint8_t r, std::uint8_t g, std::uint8_t b) noexcept
            : r(r), g(g), b(b) { }

    constexpr rgb_color(std::uint32_t hex) noexcept
            : r((hex>>16U) & 0xFF), g((hex>>8U) & 0xFF), b(hex & 0xFF) { }

    template <rgb_color_enum T>
    constexpr rgb_color(T c) noexcept
            : rgb_color(static_cast<std::uint32_t>(c)) {};

    constexpr rgb_color(const rgb_color&) noexcept = default;

    constexpr rgb_color(rgb_color&&) noexcept = default;

    constexpr rgb_color& operator=(const rgb_color&) noexcept = default;

    constexpr rgb_color& operator=(rgb_color&&) noexcept = default;

    explicit constexpr operator std::uint32_t() const noexcept
    {
        return ((std::uint32_t(r)) << 16U & 0xFF0000U) |
               ((std::uint32_t(g)) << 8U  & 0x00FF00U) |
               ((std::uint32_t(b)) << 0U  & 0x0000FFU);
    }

    std::uint8_t r, g, b;

    inline friend constexpr bool operator==(rgb_color lhs, rgb_color rhs) noexcept
    { return lhs.r == rhs.r && lhs.g == rhs.g && lhs.b == rhs.b; }

    inline friend constexpr bool operator!=(rgb_color lhs, rgb_color rhs) noexcept
    { return lhs.r != rhs.r || lhs.g != rhs.g || lhs.b != rhs.b; }

    inline friend constexpr rgb_color operator|(rgb_color lhs, rgb_color rhs) noexcept
    { return rgb_color(std::uint32_t(lhs) | std::uint32_t(rhs)); }
};

TERMCONTROL_DEFINE_MAX_FORMATTED_SIZE(rgb_color, 2 + 3 * detail::max_formatted_size_v<std::uint8_t>);


//namespace detail {

/// Class representing a color.
class color_type
{
public:
    constexpr color_type()
            : color_type(terminal_color::reset) { }

    constexpr color_type(terminal_color c)                              // NOLINT
            : color_(std::in_place_type<terminal_color>, c) { }

    constexpr color_type(rgb_color c)                                   // NOLINT
            : color_(c) { }

    constexpr color_type(std::uint8_t r, std::uint8_t g, std::uint8_t b) // NOLINT
            : color_(std::in_place_type<rgb_color>, r, g, b) { }

    constexpr color_type(std::uint32_t hex)                             // NOLINT
            : color_(std::in_place_type<rgb_color>, hex) { }

    constexpr auto is_rgb_color() -> bool
    { return std::holds_alternative<rgb_color>(color_); };

    constexpr auto is_terminal_color() -> bool
    { return std::holds_alternative<terminal_color>(color_); };

    constexpr auto get_rgb_color() -> rgb_color
    { return std::get<rgb_color>(color_); }

    constexpr auto get_terminal_color() -> terminal_color
    { return std::get<terminal_color>(color_); }

private:
    std::variant<terminal_color, rgb_color> color_;
};

} // namespace termcontrol