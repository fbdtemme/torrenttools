//s//
//// Created by fbdtemme on 05/01/2020.
////
//
//#include <thread>
//#include <chrono>
//#include <utility>
//#include <string>
//#include <iostream>
//#include <sstream>
//
//#include <termcontrol/format.hpp>
//#include <termcontrol/ostream.hpp>
//#include <cliprogress/progress.hpp>
//
//#include <cliprogress/widgets/rate.hpp>
//#include <cliprogress/widgets/eta.hpp>
//
//using namespace std::chrono_literals;
//
//cliprogress::progress_bar_options asci_progress_example()
//{
//    cliprogress::progress_bar_options options {};
//
//    // Define the symbols to use to draw the progress bar
//    options.symbols = cliprogress::bar_symbols {
//            .left_seperator = '[',
//            .right_seperator = ']',
//            .incomplete = '.',
//            .complete = '#',
//            .complete_lead = '#',
//    };
//
//    // Create the format string to define what information
//    // to include in the progress bar and the formatting.
//    std::stringstream ss {};
//    ss << bg(termcontrol::terminal_color::green)
//       << " Progress: [{percentage:3.0f}%]"
//       << bg(termcontrol::terminal_color::reset)
//       << " {bar}    ";
//
//    options.format_string = ss.str();
//    return options;
//}
//
//cliprogress::progress_bar_options eta_and_rate_widget_example()
//{
//    cliprogress::progress_bar_options options {};
//
//    // Create the format string to define what information
//    // to include in the progress bar and the formatting.
//    std::stringstream ss {};
//    ss << bg(termcontrol::terminal_color::green)
//       << " Progress: [{percentage:3.0f}%]"
//       << bg(termcontrol::terminal_color::reset)
//       << " {bar} {rate} | {eta} ";
//
//    options.widgets.add("rate", std::make_unique<cliprogress::ewma_rate>("it/s"));
//    options.widgets.add("eta",  std::make_unique<cliprogress::smoothed_eta>());
//
//    options.format_string = ss.str();
//    return options;
//}
//
//
//void drive_bar(cliprogress::progress_bar_options& options)
//{
//    cliprogress::progress_bar bar(options);
//
//    for (std::size_t i = 0; i <= 100; ++i) {
//        bar.set_progress(double(i));
//        std::this_thread::sleep_for(50ms);
//    }
//    bar.set_progress(100);
//    bar.clear();
//    std::cout << "All done.\n";
//}
//
//
//
////
////void unicode_example()
////{
////    cliprogress::progress_bar_options options {};
////
////    options.symbols = cliprogress::symbols_unicode {
////            .left_seperator = '|',
////            .right_seperator = '|',
////            .incomplete = ' ',
////            .complete_frames = cliprogress::progress_preset::vertical_blocks
////    };
////    options.style = cliprogress::bar_style {
////            .seperator = {},
////            .complete = bg(termcontrol::terminal_color::white),
////            .incomplete = bg(termcontrol::terminal_color::white),
////    };
////
////    options.format_string = "{percentage:3.0f}% | {elapsed:%T} {bar} {eta:%T}";
////
////    cliprogress::global_progress_bar bar(0, 50, options);
////
////    using namespace std::chrono_literals;
////    int task = 1;
////
////    for (std::size_t i = 0; i < 50; ++i) {
////        bar.progress(double(i));
////        if (i % 10 == 0) {
////            std::cout << "Task " << task << "...\n";
////            task += 1;
////        }
////        std::this_thread::sleep_for(100ms);
////    }
////    bar.clear();
////    std::cout << "All done.\n";
////    std::this_thread::sleep_for(2s);
////}
////
////void custom_label_example()
////{
////    cliprogress::progress_bar_options options {};
////
////    options.symbols = cliprogress::symbols_unicode{};
////
////    options.style = cliprogress::bar_style {
////            .seperator = {},
////            .complete = bg(termcontrol::terminal_color::white),
////            .incomplete = bg(termcontrol::terminal_color::white),
////    };
////
////    options.format_string = "{percentage:3.0f}% | {my_label} {bar} {eta:%T}";
////    cliprogress::data_label label("my_label", [](const auto& data) {
////        return fmt::format("{} seconds", data.elapsed.count());
////    });
////
////    // custom_labels is a span we pass only a single pointer
////    options.custom_labels = std::span(&label, 1);
////
////    cliprogress::global_progress_bar bar(0, 200, options);
////
////    using namespace std::chrono_literals;
////    int task = 1;
////
////    for (std::size_t i = 0; i < 200; ++i) {
////        bar.progress(double(i));
////        if (i % 10 == 0) {
////            std::cout << "Task " << task << "...\n";
////            task += 1;
////        }
////        std::this_thread::sleep_for(30ms);
////    }
////    bar.clear();
////    std::cout << "All done.\n";
////    std::this_thread::sleep_for(2s);
////}
//
//
//int main()
//{
//    std::setlocale(LC_ALL, "");
//    auto options = eta_and_rate_widget_example();
//    drive_bar(options);
//    return EXIT_SUCCESS;
//}