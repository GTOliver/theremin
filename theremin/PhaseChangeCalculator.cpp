#include "PhaseChangeCalculator.h"

#include <algorithm>

#include "juce_audio_utils/juce_audio_utils.h"

void PhaseChangeCalculator::prepare(double sample_rate)
{
    sample_rate_ = sample_rate;
}

double PhaseChangeCalculator::calculate(double position)
{
    double min_position = -0.5 * range_;

    double fraction = (position - min_position) / range_;
    fraction = std::clamp(fraction, 0.0, 1.0);

    double freq = fraction * (max_freq_ - min_freq_) + min_freq_;
    return frequency_to_phase_change(freq);
}

double PhaseChangeCalculator::frequency_to_phase_change(double frequency) const
{
    if (sample_rate_ == 0.0)
        return 0.0;

    double cycles_per_sample = frequency / sample_rate_;
    return cycles_per_sample * 2.0 * juce::MathConstants<double>::pi;
}
