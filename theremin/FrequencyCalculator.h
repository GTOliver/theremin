#pragma once

#include "Bounds.h"

class FrequencyCalculator
{
public:
    double calculate(double position);

    void set_distance_range(double range);

    void set_frequency_bounds(Bounds bounds);

private:
    double range_{0.5};
    Bounds frequency_{500, 2000};
};
