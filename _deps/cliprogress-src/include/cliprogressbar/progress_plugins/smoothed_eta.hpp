#pragma once
#include <chrono>
#include <string_view>
#include <memory>
#include <any>
#include <chrono>
#include <span>

#include "cliprogressbar/progress_data.hpp"

namespace cliprogress {

using namespace std::chrono_literals;

class smoothed_eta : public eta_strategy
{
public:
    smoothed_eta(std::chrono::milliseconds burn_in_period = 100ms) noexcept
        : burn_in_period_(burn_in_period)
    {}

    void update(progress_data& progress) override
    {
        using double_duration =  std::chrono::duration<double>;
        double rate = progress.rate();
        /// rate is unset
        if (rate == std::numeric_limits<double>::infinity())
            return;

        if (rate == 0) {
            eta_ = std::numeric_limits<duration>::max();
            return;
        }

        previous_eta_ = eta_;
        // lineair interpolation from the current rate
        auto eta_guess = double_duration((progress.max_value() - progress.value()) / rate);

        // Apply smoothing only if the new time isn't a huge change -- e.g., if the
        // new time is more than half of the previous time; this is useful for slow starts.
        if (eta_guess > (previous_eta_ / 3) && previous_eta_ > 0s) {
            // Apply hysteresis to favor downward over upward swings
            // 30% of down and 10% of up (exponential smoothing)
            auto previous_eta = std::chrono::duration_cast<double_duration>(previous_eta_);
            auto diff = eta_guess.count() - previous_eta.count();
            eta_guess = previous_eta + double_duration((diff < 0 ? 0.3 : 0.1) * diff);
        }

        eta_guess = std::max(double_duration(0), eta_guess);
        eta_ = std::chrono::duration_cast<duration>(eta_guess);

        // only return actual eta if the burn in period is over.
        if (!burn_in_period_over_ && (progress.time_elapsed() > burn_in_period_ )) [[likely]] {
            burn_in_period_over_ = true;
        }
    }

    /// Return the expected time untill completion
    /// Return a negative duration if the eta is not determined.
    virtual duration eta() const override
    {
        // only return actual eta if the burn in period is over.
        if (burn_in_period_over_) [[likely]] { return eta_; }
        return std::numeric_limits<duration>::max();
    }

private:
    duration previous_eta_;
    duration eta_;
    std::chrono::milliseconds burn_in_period_;
    bool burn_in_period_over_;
};

} // namespace cliprogress