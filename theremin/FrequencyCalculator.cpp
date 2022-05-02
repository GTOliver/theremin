#include "FrequencyCalculator.h"

#include <algorithm>

double FrequencyCalculator::calculate(double position)
{
    std::scoped_lock lock(mtx_);
    double distance_min = -0.5 * range_;
    double fraction = (position - distance_min) / range_;
    fraction = std::clamp(fraction, 0.0, 1.0);

    return fraction * (frequency_.max() - frequency_.min()) + frequency_.min();
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
