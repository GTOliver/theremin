#include "FrequencyCalculator.h"

#include <algorithm>

double FrequencyCalculator::calculate(double position)
{
    double min_position = -0.5 * range_;
    double fraction = (position - min_position) / range_;
    fraction = std::clamp(fraction, 0.0, 1.0);

    double freq = fraction * (max_freq_ - min_freq_) + min_freq_;
    return freq;
}
