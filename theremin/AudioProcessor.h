#pragma once

#include <juce_audio_utils/juce_audio_utils.h>

#include "ThereMessage.h"

class AudioProcessor
{
public:
    void prepare(double sample_rate);

    void get_next_audio_block(const juce::AudioSourceChannelInfo& audio_data);

    void process(ThereMessage message);

    void set_attack(double attack);
    void set_decay(double decay);
    void set_sustain(double sustain);
    void set_release(double release);

private:
    enum class EnvelopeState
    {
        Attack,
        Decay,
        Sustain,
        Release,
        Off
    };

    [[nodiscard]] double calculate_rate(double duration) const;

    void update_attack_target();

    [[nodiscard]] double frequency_to_phase_change(double frequency) const;

    [[nodiscard]] float next_value();

    std::atomic<double> attack_{100.0};
    double attack_rate_{0.0};
    double specific_attack_rate_{0.0};
    double attack_target_{1.0};
    std::atomic<double> decay_{200.0};
    double decay_rate_{0.0};
    double specific_decay_rate_{0.0};
    std::atomic<double> sustain_{0.8};
    std::atomic<double> release_{500.0};
    double release_rate_{0.0};
    double specific_release_rate_{0.0};

    double sample_rate_{0.0};

    double level_{0.0};
    double target_level_{0.0};

    double level_change_rate_{0.0};
    double level_ramp_time_ms_{10.0};

    EnvelopeState state_{EnvelopeState::Off};
    double envelope_level_{0.0};

    double phase_change_per_sample_{0.0};
    double phase_change_per_sample_target_{0.0};
    double phase_change_ramp_time_ms_{5.0};
    double phase_change_rate_{0.0};

    double phase_{0.0};
};
