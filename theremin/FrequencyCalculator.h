#pragma once

#include <mutex>
#include <vector>

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

enum class SnappingMode
{
    None,
    Chromatic,
    Major,
    Pentatonic
};

class FrequencyCalculator
{
public:
    double calculate(double position) const;

    void set_distance_range(double range);

    void set_frequency_bounds(Bounds bounds);

    void set_scaling_method(ScalingMethod method);

    void set_snapping_mode(SnappingMode mode);

private:
    double calculate_scaled_frequency(double input) const;

    double freq_to_note_number(double freq) const;

    double note_number_to_freq(double n) const;

    double snap_to_chromatic(double freq) const;

    double snap_to_scale(const std::vector<int>& scale, double freq) const;

    mutable std::mutex mtx_;
    double range_{0.5};
    Bounds frequency_{500, 2000};
    ScalingMethod scaling_method_{ScalingMethod::Linear};
    SnappingMode snapping_mode_{SnappingMode::None};

    double snapping_f0_{440.0};
    int snapping_N_{12};

    const std::vector<int> major_scale_{0, 2, 4, 5, 7, 9, 11};
    const std::vector<int> pentatonic_major_scale_{0, 2, 4, 7, 9};
};
