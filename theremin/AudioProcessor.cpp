#include "juce_audio_utils/juce_audio_utils.h"

#include "AudioProcessor.h"

void AudioProcessor::prepare(double sample_rate)
{
    sample_rate_ = sample_rate;

    // Update rates
    set_attack(attack_);
    set_decay(decay_);
    set_release(release_);
}

void AudioProcessor::get_next_audio_block(const juce::AudioSourceChannelInfo& audio_data)
{
    auto* leftBuffer = audio_data.buffer->getWritePointer(0, audio_data.startSample);
    auto* rightBuffer = audio_data.buffer->getWritePointer(1, audio_data.startSample);

    for (auto sample = 0; sample < audio_data.numSamples; ++sample) {
        auto current_value = next_value();
        leftBuffer[sample] = current_value;
        rightBuffer[sample] = current_value;
    }

    phase_ = std::fmod(phase_, 2.0 * juce::MathConstants<double>::pi);
}

void AudioProcessor::process(ThereMessage message)
{
    if (message.off) {
        state_ = EnvelopeState::Release;
        specific_release_rate_ = envelope_level_ * release_rate_;
        return;
    }

    bool currently_off = (state_ == EnvelopeState::Release || state_ == EnvelopeState::Off);

    if (message.note_change || (currently_off && message.level > 0.0)) {
        if (attack_.load() == 0.0) {
            state_ = EnvelopeState::Sustain;
        } else {
            state_ = EnvelopeState::Attack;
            specific_attack_rate_ = attack_rate_ * (attack_target_ - envelope_level_);
        }
    }

    target_level_ = message.level;
    level_change_rate_ = calculate_rate(level_ramp_time_ms_) * (target_level_ - level_);
    phase_change_per_sample_target_ = frequency_to_phase_change(message.frequency);
    phase_change_rate_ = calculate_rate(phase_change_ramp_time_ms_) * (phase_change_per_sample_target_ - phase_change_per_sample_);

    if (enable_cc_) {
        set_square_blend(message.cc);
    }
}

void AudioProcessor::set_attack(double attack)
{
    attack_.store(attack);
    attack_rate_ = calculate_rate(attack);
}

void AudioProcessor::set_decay(double decay)
{
    decay_.store(decay);
    decay_rate_ = calculate_rate(decay);

    update_attack_target();
}

void AudioProcessor::set_sustain(double sustain)
{
    sustain_.store(sustain);
    update_attack_target();
}

void AudioProcessor::set_release(double release)
{
    release_.store(release);
    release_rate_ = calculate_rate(release);
}

void AudioProcessor::set_enable_cc(bool enabled)
{
    enable_cc_.store(enabled);
}

void AudioProcessor::set_square_blend(double amount)
{
    target_blend_.store(amount);
    blend_change_rate_ = calculate_rate(blend_ramp_time_ms_) * (target_blend_ - oscillator_.get_square_blend());
}

double AudioProcessor::calculate_rate(double duration) const
{
    if (duration == 0.0) {
        return 0.0;
    }
    return 1000.0 / (duration * sample_rate_);
}

void AudioProcessor::update_attack_target()
{
    attack_target_ = decay_rate_ == 0.0 ? sustain_.load() : 1.0;
}

double AudioProcessor::frequency_to_phase_change(double frequency) const
{
    if (sample_rate_ == 0.0)
        return 0.0;

    double cycles_per_sample = frequency / sample_rate_;
    return cycles_per_sample * 2.0 * juce::MathConstants<double>::pi;
}

float AudioProcessor::next_value()
{
    auto oscillator_value = oscillator_.evaluate_at(phase_);

    auto return_value = level_ * envelope_level_ * oscillator_value;

    if (phase_change_rate_ != 0.0) {
        int sign = (phase_change_per_sample_target_ >= phase_change_per_sample_) ? 1 : -1;
        phase_change_per_sample_ += phase_change_rate_;
        if ((phase_change_per_sample_ - phase_change_per_sample_target_) * sign > 0) {
            phase_change_per_sample_ = phase_change_per_sample_target_;
            phase_change_rate_ = 0.0f;
        }
    }

    phase_ += phase_change_per_sample_;

    if (phase_ > 2.0 * juce::MathConstants<double>::pi) {
        phase_ -= 2.0 * juce::MathConstants<double>::pi;
    }

    if (level_change_rate_ != 0.0) {
        int sign = (target_level_ >= level_) ? 1 : -1;
        level_ += level_change_rate_;
        if ((level_ - target_level_) * sign > 0) {
            level_ = target_level_;
            level_change_rate_ = 0.0;
        }
    }

    if (blend_change_rate_ != 0.0) {
        double current_blend = oscillator_.get_square_blend();
        int sign = (target_blend_ >= current_blend) ? 1 : -1;
        double new_blend = current_blend + blend_change_rate_;
        if ((new_blend - target_blend_) * sign > 0) {
            new_blend = target_blend_;
            blend_change_rate_ = 0.0;
        }
        oscillator_.set_square_blend(new_blend);
    }

    switch (state_) {
        case (EnvelopeState::Attack): {
            envelope_level_ += specific_attack_rate_;
            if (envelope_level_ >= attack_target_) {
                envelope_level_ = attack_target_;
                if (decay_rate_ > 0.0) {
                    state_ = EnvelopeState::Decay;
                    specific_decay_rate_ = (attack_target_ - sustain_) * decay_rate_;
                } else {
                    envelope_level_ = sustain_;
                }
            }
            break;
        }
        case (EnvelopeState::Decay): {
            envelope_level_ -= specific_decay_rate_;
            if (envelope_level_ <= sustain_) {
                envelope_level_ = sustain_;
                state_ = EnvelopeState::Sustain;
            }
            break;
        }
        case (EnvelopeState::Sustain): {
            envelope_level_ = sustain_;
            break;
        }
        case (EnvelopeState::Release): {
            envelope_level_ -= specific_release_rate_;
            if (envelope_level_ <= 0.0) {
                envelope_level_ = 0.0;
                state_ = EnvelopeState::Off;
            }
            break;
        }
        default:
            envelope_level_ = 0.0;
            break;
    }
    return static_cast<float>(return_value);
}
