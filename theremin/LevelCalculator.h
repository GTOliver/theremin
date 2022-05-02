#pragma once

#include "Bounds.h"

class LevelCalculator
{
public:
    [[nodiscard]] double calculate(double distance) const;

    void set_max_level(double level);

    void set_distance_bounds(Bounds bounds);

private:
    double max_volume_{0.2};
    Bounds distance_{0.15, 0.5};
};
