#pragma once
#include <any>
#include <atomic>
#include <chrono>
#include <limits>
#include <map>
#include <memory>
#include <set>
#include <span>
#include <string_view>

#include <gsl-lite/gsl-lite.hpp>
#include <fmt/format.h>
#include <fmt/chrono.h>

#include <cmath>


namespace cliprogress {


/// Class holding the progress of determinate progress indicators.

class progress_data;

class progress_data_strategy
{
public:
    using duration = std::chrono::system_clock::duration;
    using time_point = std::chrono::system_clock::time_point;

    virtual void update(progress_data& data) = 0;
    virtual ~progress_data_strategy() = default;
};


class rate_strategy : public progress_data_strategy
{
public:
    virtual double rate() const = 0;
};

class eta_strategy : public progress_data_strategy
{
public:
    virtual duration eta() const = 0;
};



class progress_data
{
public:
    progress_data(double min_value, double max_value)
        : min_value_(min_value)
        , max_value_(max_value)
        , value_(min_value)
    {}

    progress_data()
            : progress_data(0, 100) {};

    void set_range(double min_value, double max_value)
    {
        min_value_ = min_value;
        max_value_ = max_value;
        value_.store(
                std::clamp(value_.load(std::memory_order_relaxed), min_value, max_value));
    }

    void set_rate_stategy(std::unique_ptr<rate_strategy> strategy)
    {
        rate_stategy_ = std::move(strategy);
    }

    void set_eta_stategy(std::unique_ptr<eta_strategy> strategy)
    {
        eta_strategy_ = std::move(strategy);
    }

    using duration = std::chrono::system_clock::duration;
    using time_point = std::chrono::system_clock::time_point;

    void update(double new_value) noexcept
    {
        Expects(new_value <= max_value_);
        auto t = std::chrono::system_clock::now();

        // Update base progress data
        if (bool ts = time_started() == time_point(); ts) [[unlikely]] {
            time_started_.store(t, std::memory_order_relaxed);
            return;
        }

        if (value() < max_value()) [[likely]] {
            time_current_.store(t, std::memory_order_relaxed);
        }
        else if (time_stopped() == time_point()){
            time_stopped_.store(t, std::memory_order_relaxed);
        }

        value_.store(std::min(new_value, max_value()), std::memory_order_relaxed);

        // handle zero byte progress ranges
        if (min_value() == max_value()) {
            percentage_ = 100;
        }
        else {
            percentage_ = (value()-min_value()) / (max_value()-min_value()) * 100;
        }

        if (rate_stategy_)
            rate_stategy_->update(*this);

        if (eta_strategy_)
            eta_strategy_->update(*this);
    }

    double min_value() const
    {
        return min_value_.load(std::memory_order_relaxed);
    }

    double max_value() const
    {
        return max_value_.load(std::memory_order_relaxed);
    }

    double value() const
    {
        return value_.load(std::memory_order_relaxed);
    }

    double percentage() const
    {
        return percentage_.load(std::memory_order_relaxed);
    }

    time_point time_started() const
    {
        return time_started_.load(std::memory_order_relaxed);
    }

    time_point time_stopped() const
    {
        return time_stopped_.load(std::memory_order_relaxed);
    }

    duration time_elapsed() const
    {
        return time_current_.load(std::memory_order_relaxed) - time_started();
    }

    double rate() const
    {
        if (!rate_stategy_) return 0;
        return rate_stategy_->rate();
    }

    duration eta() const
    {
        if (!eta_strategy_) return std::numeric_limits<duration>::max();
        return eta_strategy_->eta();
    }

private:
    std::atomic<double> min_value_ = 0;
    std::atomic<double> max_value_ = 100;
    std::atomic<double> value_ = 0;
    std::atomic<double> percentage_ = 0;

    std::atomic<time_point> time_started_ {};
    std::atomic<time_point> time_stopped_ {};
    std::atomic<time_point> time_current_ {};

    std::unique_ptr<rate_strategy> rate_stategy_ {};
    std::unique_ptr<eta_strategy> eta_strategy_ {};

};

}

