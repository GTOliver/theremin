#include "LevelCalculator.h"

#include <algorithm>

double LevelCalculator::calculate(double distance) const
{
    double fraction = (distance - distance_.min()) / (distance_.max() - distance_.min());
    fraction = std::clamp(fraction, 0.0, 1.0);

    return max_volume_ * fraction;
}

void LevelCalculator::set_max_level(double level)
{
    max_volume_ = level;
}

void LevelCalculator::set_distance_bounds(Bounds bounds)
{
    distance_ = std::move(bounds);
}
