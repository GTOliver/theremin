#pragma once

#include "Bounds.h"

#include <mutex>

class LevelCalculator
{
public:
    [[nodiscard]] double calculate(double distance) const;

    void set_level_bounds(Bounds bounds);

    void set_distance_bounds(Bounds bounds);

private:
    mutable std::mutex mtx_;

    Bounds volume_bounds_{0.0, 0.2};
    Bounds distance_{0.15, 0.5};
};
