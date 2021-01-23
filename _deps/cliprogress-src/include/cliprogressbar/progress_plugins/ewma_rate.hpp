#pragma once
#include <chrono>
#include <string_view>
#include <memory>
#include <any>
#include <chrono>
#include <span>
#include <algorithm>
#include <numeric>
#include <deque>

#include "cliprogressbar/progress_data.hpp"

namespace cliprogress {

using namespace std::chrono_literals;

/// Widget base class calculating the exponentially weigthed moving average (ewma) rate of change.
class ewma_rate : public rate_strategy
{
public:
    /// @param alpha: smoothing factor
    /// @param n: number of previous values to use in smoothing.
    explicit ewma_rate(double alpha = 0.70, std::size_t n = 10,
              std::chrono::milliseconds burn_in_period = 100ms) noexcept
            : alpha_(alpha)
            , history_max_size_(n)
            , burn_in_period_(burn_in_period)
    {}

    void update(progress_data& progress) override
    {
        using namespace std::chrono_literals;
        using ds = std::chrono::duration<double>;

        auto time_elapsed = progress.time_elapsed();
        auto elapsed_long = time_elapsed - previous_elapsed_;
        auto elapsed_double = std::chrono::duration_cast<ds>(elapsed_long).count();

        if (elapsed_double < 0.005) [[unlikely]]
            return;

//        Expects(progress.value() >= previous_value_);

        auto current_rate = (progress.value() - previous_value_) / elapsed_double;
        auto history_sum = std::reduce(history_.begin(), history_.end());
        auto history_average = history_.size() ? history_sum / history_.size() : 0;
        // make sure the rate is positive, this can be negative it the progress value
        // decreases for some weird reason
        rate_ = std::max(0., (current_rate * alpha_) + (history_average * (1-alpha_)));

        if (history_.size() == history_max_size_) {
            history_.pop_front();
        }

        history_.push_back(rate_);
        previous_elapsed_ = time_elapsed;
        previous_value_ = progress.value();

        // only return actual eta if the burn in period is over.
        if (!burn_in_period_over_ && time_elapsed > burn_in_period_ ) [[likely]] {
            burn_in_period_over_ = true;
        }
    }

    double rate() const override
    {
        // only return actual eta if the burn in period is over.
        if (burn_in_period_over_) [[likely]] return rate_;
        return 0;
    }

private:
    double rate_ = 0;
    double previous_value_ = 0;
    duration previous_elapsed_ {};
    std::size_t history_max_size_ {};
    double alpha_;
    std::chrono::milliseconds burn_in_period_;
    bool burn_in_period_over_ = false;
    std::deque<double> history_ {};
};

}