#include "FrequencyCalculator.h"

#include <algorithm>
#include <cmath>

double FrequencyCalculator::calculate(double position) const
{
    std::scoped_lock lock(mtx_);
    double distance_min = -0.5 * range_;
    double fraction = (position - distance_min) / range_;
    fraction = std::clamp(fraction, 0.0, 1.0);

    return calculate_scaled_frequency(fraction);
}

void FrequencyCalculator::set_distance_range(double range)
{
    std::scoped_lock lock(mtx_);
    range_ = range;
}

void FrequencyCalculator::set_frequency_bounds(Bounds bounds)
{
    std::scoped_lock lock(mtx_);
    frequency_ = std::move(bounds);
}

void FrequencyCalculator::set_scaling_method(ScalingMethod method)
{
    std::scoped_lock lock(mtx_);
    scaling_method_ = method;
}

double FrequencyCalculator::calculate_scaled_frequency(double progress) const
{
    switch (scaling_method_) {
        case (ScalingMethod::Linear): {
            return progress * (frequency_.max() - frequency_.min()) + frequency_.min();
        }
        case (ScalingMethod::Keyboard): {
            return frequency_.min() * std::pow(frequency_.max() / frequency_.min(), progress);
        }
        case (ScalingMethod::String): {
            return frequency_.min() / (1.0 - progress * (1.0 - (frequency_.min() / frequency_.max())));
        }
        case (ScalingMethod::Theremin): {
            double ratio = frequency_.min() / frequency_.max();
            double b = 1.0 / (1.0 - (ratio * ratio));
            double a = frequency_.min() * std::sqrt(b);
            return a * std::pow(b - progress, -0.5);
        }
        default:
            // This should never happen
            return frequency_.min();
    }
}
