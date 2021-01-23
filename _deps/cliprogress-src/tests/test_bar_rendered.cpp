////
//// Created by fbdtemme on 3/14/20.
////
//
//#include <catch2/catch.hpp>
//
//#include <cliprogress/detail/bar.hpp>
//#include <cliprogress/presets.hpp>
//
//#include <fmt/format.h>
//
//
//TEST_CASE("test bar rendered")
//{
//    using namespace cliprogress;
//
//    bar_symbols asci_symbols {
//            .left_seperator = '[',
//            .right_seperator = ']',
//            .incomplete = '.',
//            .complete = '#',
//    };
//
//    bar_symbols unicode_symbols {
//        .left_seperator =  U'│',
//        .right_seperator = U'│',
//        .incomplete = U'·',
//        .complete = U'█',
//        .complete_lead = U'➢'
//    };
//
//    bar_style style{};
//
//    SECTION("test encoded symbols") {
//        SECTION("asci symbols") {
//            detail::bar_renderer bar_renderer(asci_symbols, style);
//
//            CHECK(bar_renderer.get_symbol_complete() == "#");
//            CHECK(bar_renderer.get_symbol_incomplete() == ".");
//            CHECK(bar_renderer.get_symbol_left_seperator() == "[");
//            CHECK(bar_renderer.get_symbol_right_seperator() == "]");
//            CHECK(bar_renderer.get_symbol_complete_lead() == "#");
//        }
//
//        SECTION("unicode symbols") {
//            detail::bar_renderer bar_renderer(unicode_symbols, style);
//
//            CHECK(bar_renderer.get_symbol_complete() == "█");
//            CHECK(bar_renderer.get_symbol_incomplete() == "·");
//            CHECK(bar_renderer.get_symbol_left_seperator() == "│");
//            CHECK(bar_renderer.get_symbol_right_seperator() == "│");
//            CHECK(bar_renderer.get_symbol_complete_lead() == "➢");
//        }
//    }
//
//    SECTION("test encoded style's") {
//        SECTION("no style") {
//            detail::bar_renderer bar_renderer(asci_symbols, {});
//
//            CHECK(bar_renderer.get_style_complete() == "");
//            CHECK(bar_renderer.get_style_incomplete() == "");
//        }
//    }
//}
//
//
//TEST_CASE("label formatter")
//{
//    using namespace cliprogress;
//
//    SECTION("no {bar} label") {
//        auto hash_rate_label = cliprogress::data_label {
//                "hash_rate", [](const cliprogress::progress_data& value) {
//                    return fmt::format("{} TEST", value.rate);
//                }
//        };
//
//        detail::widget_formatter formatter {" {percentage:3.0f}% | {hash_rate} ", std::span(&hash_rate_label, 1)};
//
//    }
//}