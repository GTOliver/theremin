#include "LevelCalculator.h"

#include <algorithm>

double LevelCalculator::calculate(double distance) const
{
    double range = max_distance_ - min_distance_;
    double fraction = (distance - min_distance_) / range;

    fraction = std::clamp(fraction, 0.0, 1.0);

    return max_volume_ * fraction;
}
