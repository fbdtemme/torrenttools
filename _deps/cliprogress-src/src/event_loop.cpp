//
// Created by fbdtemme on 9/7/20.
//
#include "cliprogressbar/event_loop.hpp"

namespace cliprogress
{


void event_loop::start()
{
    // queue already running
    if (event_loop_.joinable()) return;
    event_loop_ = std::jthread(&event_loop::main_loop, this);
}

void event_loop::stop()
{
    event_loop_.request_stop();
    if (event_loop_.joinable())
        event_loop_.join();
}

void event_loop::main_loop()
{
    ///
    event_item item;
    event_item resize_item;

    auto stop_token = event_loop_.get_stop_token();

    while (!stop_token.stop_requested() && stop_token.stop_possible()) {
//         try to collapse resize events in the queue

        event_queue_.pop(item);

        if (item.value) {
            if (auto dest = item.destination.lock(); dest) {
                dest->on_event(std::move(item.value));
            }
            else {
                for_each_widget([&](const auto& w) {
                    w->on_event(item.value);
                });
            }
        }
    }
    frame_clock_.start();
    terminal_size_clock_.start();


    frame_clock_.stop();
    terminal_size_clock_.stop();

    // pump queue empty

    while(event_queue_.try_pop(item)) {
        if (item.value) {
            if (auto dest = item.destination.lock(); dest) {
                dest->on_event(std::move(item.value));
            } else {
                for_each_widget([&](const auto& w) { w->on_event(item.value); });
            }
        }
    }
}

}