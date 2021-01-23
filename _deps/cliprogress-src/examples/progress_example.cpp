//
// Created by fbdtemme on 7/10/20.
//


#include <chrono>
#include <thread>
#include "cliprogressbar/widgets/bar.hpp"
#include "cliprogressbar/widgets/label.hpp"
#include "cliprogressbar/widgets/animation.hpp"

#include "cliprogressbar/progress_indicator.hpp"
#include <cliprogressbar/progress_plugins/ewma_rate.hpp>
#include <cliprogressbar/progress_plugins/smoothed_eta.hpp>
#include "cliprogressbar/formatters.hpp"

#include <iostream>
#include <sstream>


int main()
{
    std::setlocale(LC_ALL, "");
    using namespace std::chrono_literals;
    cliprogress::application app;

    cliprogress::progress_indicator indicator(&app);
    indicator.set_progress_range(0, 100);

    indicator.set_rate_strategy(std::make_unique<cliprogress::ewma_rate>());
    indicator.set_eta_strategy(std::make_unique<cliprogress::smoothed_eta>());

    auto perc    = std::make_shared<cliprogress::label>();
    auto spinner = std::make_shared<cliprogress::animation>(cliprogress::animations::dots);
    auto bar     = std::make_shared<cliprogress::bar>(
            cliprogress::bar_symbols{
                .complete_frames = std::span(cliprogress::bar_frames::horizontal_blocks)},
            cliprogress::bar_style{});

    auto rate_label = std::make_shared<cliprogress::label>();
    auto eta_label     = std::make_shared<cliprogress::label>();

    indicator.value_changed.connect(
            [=](const cliprogress::progress_data* data) {
                perc->set_text(fmt::format("percentage: {:>3.0f}", data->percentage()));
            });
    indicator.value_changed.connect(
            [=](const cliprogress::progress_data* data) {
                rate_label->set_text(cliprogress::format_binary_unit(data->rate(), "B/s"));
            });
    indicator.value_changed.connect(
            [=](const cliprogress::progress_data* data) {
                eta_label->set_text(
                        fmt::format("    {:%M:%S}    ",
                                std::chrono::duration_cast<std::chrono::seconds>(data->eta())));
            });
    indicator.value_changed.connect(
            [=](const cliprogress::progress_data* data) {
                bar->set_percentage(data->percentage());
            });

    indicator.push_back(perc);
    indicator.push_back(spinner);
    indicator.push_back(bar);
    indicator.set_spacing(1);
    indicator.push_back(rate_label);
    indicator.push_back(eta_label);

    // start the main event loop
    app.start();
    indicator.start();

    auto t = std::jthread([&]() {
        for (std::size_t i = 0; i <= 100; ++i) {
            indicator.set_value(i);
            std::this_thread::sleep_for(100ms);
        }

        indicator.stop();
        app.request_stop();
        app.wait();
    });
}