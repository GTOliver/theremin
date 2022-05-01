#include "juce_audio_utils/juce_audio_utils.h"

#include "AudioProcessor.h"

void AudioProcessor::prepare(double sample_rate)
{
    sample_rate_ = sample_rate;
}

void AudioProcessor::get_next_audio_block(const juce::AudioSourceChannelInfo& audio_data)
{
    auto* leftBuffer = audio_data.buffer->getWritePointer(0, audio_data.startSample);
    auto* rightBuffer = audio_data.buffer->getWritePointer(1, audio_data.startSample);

    for (auto sample = 0; sample < audio_data.numSamples; ++sample) {
        auto current_value = std::sin(phase_);
        phase_ += phase_change_per_sample_;
        auto fvalue = static_cast<float>(current_value * level_);
        leftBuffer[sample] = fvalue;
        rightBuffer[sample] = fvalue;
    }

    phase_ = std::fmod(phase_, 2.0 * juce::MathConstants<double>::pi);
}

void AudioProcessor::process(ThereMessage message)
{
    level_ = message.level;
    phase_change_per_sample_ = frequency_to_phase_change(message.frequency);
}

double AudioProcessor::frequency_to_phase_change(double frequency) const
{
    if (sample_rate_ == 0.0)
        return 0.0;

    double cycles_per_sample = frequency / sample_rate_;
    return cycles_per_sample * 2.0 * juce::MathConstants<double>::pi;
}
