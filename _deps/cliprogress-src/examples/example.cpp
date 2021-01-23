
#include <chrono>
#include <thread>
#include "cliprogressbar/widgets/bar.hpp"
#include "cliprogressbar/widgets/label.hpp"
#include "cliprogressbar/widgets/box_layout.hpp"
#include "cliprogressbar/widgets/animation.hpp"

#include "cliprogressbar/application.hpp"

#include <iostream>
#include <sstream>


int main()
{
    std::setlocale(LC_ALL, "");
    using namespace std::chrono_literals;
    cliprogress::application app {};

    auto box = std::make_shared<cliprogress::box_layout>();
    auto bar = std::make_shared<cliprogress::bar>(
        cliprogress::bar_symbols{}, cliprogress::bar_style{});
    auto prefix_label = std::make_shared<cliprogress::label>("prefix text");
    auto suffix_label = std::make_shared<cliprogress::label>("suffix text");
    auto spinner = std::make_shared<cliprogress::animation>(cliprogress::animations::dots);

    prefix_label->set_size(20);
    box->push_back(prefix_label);
    box->push_back(spinner);
    box->push_back(bar);
    box->set_spacing(1);
    app.set_root_widget(box);

    Expects(prefix_label->get_size_policy().can_shrink());

    auto t = std::jthread([&]() {
        for (std::size_t i = 0; i <= 200; ++i) {
            bar->set_percentage(i/2);
            prefix_label->set_text(fmt::format("{:>3.0f}%", std::round(i / 200. * 100)));
            std::this_thread::sleep_for(200ms);
        }

//        std::this_thread::sleep_for(1s);
        app.request_stop();
    });
    app.start();
}