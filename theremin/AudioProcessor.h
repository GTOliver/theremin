#pragma once

#include <juce_audio_utils/juce_audio_utils.h>

#include "ThereMessage.h"

class AudioProcessor
{
public:
    void prepare(double sample_rate);

    void get_next_audio_block(const juce::AudioSourceChannelInfo& audio_data);

    void process(ThereMessage message);

private:
    [[nodiscard]] double frequency_to_phase_change(double frequency) const;

    double sample_rate_{0.0};

    double level_{0.0};
    double phase_change_per_sample_{0.0};

    double phase_{0.0};
};
