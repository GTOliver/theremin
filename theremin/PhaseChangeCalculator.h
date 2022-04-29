#pragma once

class PhaseChangeCalculator
{
public:
    void prepare(double sample_rate);
    [[nodiscard]] double calculate(double position);

private:
    [[nodiscard]] double frequency_to_phase_change(double frequency) const;

    double sample_rate_{0};
    double range_{0.5};
    double min_freq_{500};
    double max_freq_{2000};
};
