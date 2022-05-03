#include "LevelCalculator.h"

#include <algorithm>

double LevelCalculator::calculate(double distance) const
{
    std::scoped_lock lock(mtx_);
    double fraction = (distance - distance_.min()) / (distance_.max() - distance_.min());
    fraction = std::clamp(fraction, 0.0, 1.0);

    if (fraction == 0.0) {
        return 0.0;
    }

    return volume_bounds_.min() + fraction * (volume_bounds_.max() - volume_bounds_.min());
}

void LevelCalculator::set_level_bounds(Bounds bounds)
{
    std::scoped_lock lock(mtx_);
    volume_bounds_ = bounds;
}

void LevelCalculator::set_distance_bounds(Bounds bounds)
{
    std::scoped_lock lock(mtx_);
    distance_ = std::move(bounds);
}
