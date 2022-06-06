#pragma once

#include <juce_audio_utils/juce_audio_utils.h>

#include <atomic>

class Oscillator
{
public:
    [[nodiscard]] double evaluate_at(double phase) const;

    void set_square_blend(double blend);

    [[nodiscard]] double get_square_blend();

private:
    [[nodiscard]] double sine(double phase) const;
    [[nodiscard]] double square(double phase) const;

    double square_blend_{0.0};

    double square_smoothing_{0.2 * juce::MathConstants<double>::pi};
};
