#include "Oscillator.h"

#include <algorithm>
#include <cmath>

double Oscillator::evaluate_at(double phase) const
{
    return square_blend_ * square(phase) + (1.0 - square_blend_) * sine(phase);
}

void Oscillator::set_square_blend(double amount)
{
    square_blend_ = std::clamp(amount, 0.0, 1.0);
}

double Oscillator::get_square_blend()
{
    return square_blend_;
}

double Oscillator::sine(double phase) const
{
    return std::sin(phase);
}

double Oscillator::square(double phase) const
{
    double target = 1.0;
    double relative_phase = phase;

    // Restrict phase to 0-pi and calculate the target
    if (phase > juce::MathConstants<double>::pi) {
        target = -1.0;
        relative_phase = phase - juce::MathConstants<double>::pi;
    }

    // Apply symmetry to get value between 0 and pi/2
    if (relative_phase > juce::MathConstants<double>::pi / 2.0) {
        relative_phase = juce::MathConstants<double>::pi - relative_phase;
    }

    double square_val = std::clamp(target * relative_phase / square_smoothing_, -1.0, 1.0);

    // Half this, due to square waves having twice the intensity as sine waves
    return 0.5 * square_val;
}
