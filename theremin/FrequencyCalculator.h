#pragma once

#include <cmath>
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
    // No snapping
    None,

    // Snap to the 12-tone chromatic scale
    Chromatic,

    // Snap to the 12-tone major scale
    Major,

    // Snap to the 12-tone pentatonic scale
    Pentatonic,

    // No snapping, but trigger a "new note" when changing in the chromatic scale
    NoneWithChromaticEnvelope,

    // Snap to the 17-tone chromatic scale
    Chromatic17,

    // Snap to the 17-tone major scale
    Major17,

    // Snap to the 17-tone pentatonic scale
    Pentatonic17,

    // Snap to the 11-tone pentatonic scale
    Pentatonic11,

    // Snap to the 14-tone pentatonic scale
    Pentatonic14,
};

class FrequencyCalculator
{
public:
    // Return the frequency and whether the note has snapped to a new one
    std::pair<double, bool> calculate(double position);

    void set_distance_range(double range);

    void set_frequency_bounds(Bounds bounds);

    void set_scaling_method(ScalingMethod method);

    void set_snapping_mode(SnappingMode mode);

private:
    static std::vector<int> calculate_major_scale(int large, int small);

    static std::vector<int> calculate_pentatonic_major_scale(int large, int small);

    double freq_to_note_number(int n_notes, double freq) const;

    double note_number_to_freq(int n_notes, double n) const;

    std::pair<double, int> snap_to_chromatic(int n_notes, double freq) const;

    std::pair<double, int> snap_to_scale(int n_notes, const std::vector<int>& scale, double freq) const;

    double calculate_scaled_frequency(double input) const;

    mutable std::mutex mtx_;
    double range_{0.5};
    Bounds frequency_{500, 2000};
    ScalingMethod scaling_method_{ScalingMethod::Linear};
    SnappingMode snapping_mode_{SnappingMode::None};

    double snapping_f0_{440.0};

    const std::vector<int> major_12_{calculate_major_scale(2, 1)};
    const std::vector<int> pentatonic_12_{calculate_pentatonic_major_scale(3, 2)};

    const std::vector<int> major_17_{calculate_major_scale(3, 1)};
    const std::vector<int> pentatonic_17_{calculate_pentatonic_major_scale(4, 3)};

    const std::vector<int> pentatonic_11_{calculate_pentatonic_major_scale(4, 1)};
    const std::vector<int> pentatonic_14_{calculate_pentatonic_major_scale(4, 2)};

    int n_last_snapped_{0};
};
