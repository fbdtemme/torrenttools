//#pragma once
//
//#pragma once
//#include <chrono>
//#include <string_view>
//#include <memory>
//#include <any>
//#include <chrono>
//#include <span>
//
//#include "cliprogressbar/progress_data.hpp"
//#include "cliprogressbar/progress_plugins/timer.hpp"
//
//namespace cliprogress {
//
//class timer : public timer_plugin
//{
//    using duration_type = std::chrono::system_clock::duration;
//
//    constexpr timer() noexcept = default;
//
//    void update(const progress_data& progress) override
//    {
//        if (time_started_ == time_point()) [[unlikely]] {
//            time_started_ = std::chrono::system_clock::now();
//            return;
//        }
//
//        if (progress.value < progress.max_value) [[likely]] {
//            time_current_ = std::chrono::system_clock::now();
//        }
//        else if (time_stopped_ == time_point()){
//            time_stopped_ = std::chrono::system_clock::now();
//        }
//    }
//
//    time_point time_started() const override
//    {
//        // only return actual eta if the burn in period is over.
//        if (burn_in_period_over_) [[likely]] { return eta_; }
//        return duration_type{-1}
//    }
//
//    time_point time_stopped() const override
//    {
//        // only return actual eta if the burn in period is over.
//        if (burn_in_period_over_) [[likely]] { return eta_; }
//        return duration_type{-1}
//    }
//
//    duration_type time_elapsed() const override
//    {
//        // only return actual eta if the burn in period is over.
//        if (burn_in_period_over_) [[likely]] { return eta_; }
//        return duration_type{-1}
//    }
//
//private:
//    time_point time_started_ {};
//    time_point time_stopped_ {};
//    time_point time_current_ {};
//}