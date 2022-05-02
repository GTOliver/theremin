#pragma once

#include <mutex>

#include "Bounds.h"

enum class ScalingMethod
{
    // Frequency proportional to distance
    Linear,

    // Frequency changes like shortening a string
    String,

    // Distance is proportional to semitones
    Keyboard,

    // Frequency scales like an analogue theremin
    Theremin
};

class FrequencyCalculator
{
public:
    double calculate(double position) const;

    void set_distance_range(double range);

    void set_frequency_bounds(Bounds bounds);

    void set_scaling_method(ScalingMethod method);

private:
    double calculate_scaled_frequency(double input) const;

    mutable std::mutex mtx_;
    double range_{0.5};
    Bounds frequency_{500, 2000};
    ScalingMethod scaling_method_{ScalingMethod::Linear};
};
