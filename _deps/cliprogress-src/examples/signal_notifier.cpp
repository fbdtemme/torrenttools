#include <iostream>
#include <chrono>
#include <csignal>
#include <thread>
#include "cliprogressbar/posix_signal_notifier.hpp"


int main()
{
    try {
        using namespace std::chrono_literals;
        auto notifier = cliprogress::get_posix_signal_notifier();
        notifier->connect(SIGWINCH, []() { std::cout << "sigwinch\n" << std::endl; });
        notifier->connect(SIGINT, []() { std::cout << "sigint\n" << std::endl; });
        notifier->connect(SIGSTOP, []() { std::cout << "sigstop\n" << std::endl; });

        notifier->start();
        std::this_thread::sleep_for(1s);
        notifier->request_stop();
        notifier->wait();
    }
    catch (const std::exception& e) {
        std::cout << e.what();
    }
}