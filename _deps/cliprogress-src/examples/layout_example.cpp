#include <cstdlib>

#include <chrono>
#include <thread>

#include "cliprogressbar/widgets/label.hpp"
#include "cliprogressbar/widgets/animation.hpp"
#include "cliprogressbar/widgets/bar.hpp"

#include "cliprogressbar/layout/box_layout_manager.hpp"

int main()
{
    using namespace cliprogress;
    std::setlocale(LC_ALL, "");
    using namespace std::chrono_literals;
    cliprogress::application app {};

    auto root_widget = std::make_shared<widget>();
    auto box_layout  = std::make_shared<box_layout_manager>();

    auto label     = std::make_shared<cliprogress::label>("test");
    auto animation = std::make_shared<cliprogress::animation>(cliprogress::animations::dots);
    auto bar       = std::make_shared<cliprogress::bar>(
                                    cliprogress::bar_symbols{},
                                    cliprogress::bar_style{}, 10);

    bar->set_size_policy({cliprogress::size_policy_flag::expand, 1});

    box_layout->push_back(label);
    box_layout->push_back(animation);
    box_layout->push_back(bar);

    root_widget->set_layout_manager(box_layout);
    app.add_widget(root_widget);

    app.start();
    root_widget->show();

    auto t = std::jthread([&](){
        for (int i = 0; i < 100; ++i ) {
            bar->set_percentage(i);
            std::this_thread::sleep_for(100ms);
        }
        std::this_thread::sleep_for(10s);
        app.request_stop();
        app.wait();
    });

    return EXIT_SUCCESS;
}