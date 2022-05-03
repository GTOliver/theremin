#include "FrequencyCalculator.h"

#include <algorithm>
#include <array>
#include <cmath>

std::pair<double, bool> FrequencyCalculator::calculate(double position)
{
    std::scoped_lock lock(mtx_);
    double distance_min = -0.5 * range_;
    double fraction = (position - distance_min) / range_;
    fraction = std::clamp(fraction, 0.0, 1.0);

    double freq = calculate_scaled_frequency(fraction);

    std::pair<double, int> freq_and_note_number{0.0, 0};
    switch (snapping_mode_) {
        case (SnappingMode::Chromatic):
            freq_and_note_number = snap_to_chromatic(freq);
            break;
        case (SnappingMode::Pentatonic):
            freq_and_note_number = snap_to_scale(pentatonic_major_scale_, freq);
            break;
        case (SnappingMode::Major):
            freq_and_note_number = snap_to_scale(major_scale_, freq);
            break;
        default:
            freq_and_note_number.first = freq;
    }
    bool note_is_new = freq_and_note_number.second != n_last_snapped_;
    n_last_snapped_ = freq_and_note_number.second;
    return {freq_and_note_number.first, note_is_new};
}

void FrequencyCalculator::set_distance_range(double range)
{
    std::scoped_lock lock(mtx_);
    range_ = range;
}

void FrequencyCalculator::set_frequency_bounds(Bounds bounds)
{
    std::scoped_lock lock(mtx_);
    frequency_ = std::move(bounds);
}

void FrequencyCalculator::set_scaling_method(ScalingMethod method)
{
    std::scoped_lock lock(mtx_);
    scaling_method_ = method;
}

void FrequencyCalculator::set_snapping_mode(SnappingMode mode)
{
    std::scoped_lock lock(mtx_);
    snapping_mode_ = mode;
}

double FrequencyCalculator::calculate_scaled_frequency(double progress) const
{
    switch (scaling_method_) {
        case (ScalingMethod::Linear): {
            return progress * (frequency_.max() - frequency_.min()) + frequency_.min();
        }
        case (ScalingMethod::Keyboard): {
            return frequency_.min() * std::pow(frequency_.max() / frequency_.min(), progress);
        }
        case (ScalingMethod::String): {
            return frequency_.min() / (1.0 - progress * (1.0 - (frequency_.min() / frequency_.max())));
        }
        case (ScalingMethod::Theremin): {
            double ratio = frequency_.min() / frequency_.max();
            double b = 1.0 / (1.0 - (ratio * ratio));
            double a = frequency_.min() * std::sqrt(b);
            return a * std::pow(b - progress, -0.5);
        }
        default:
            // This should never happen
            return frequency_.min();
    }
}

double FrequencyCalculator::freq_to_note_number(double freq) const
{
    return static_cast<double>(snapping_N_) * std::log2(freq / snapping_f0_);
}

double FrequencyCalculator::note_number_to_freq(double n) const
{
    return snapping_f0_ * std::pow(2.0, n / static_cast<double>(snapping_N_));
}

std::pair<double, int> FrequencyCalculator::snap_to_chromatic(double freq) const
{
    auto n = freq_to_note_number(freq);
    auto rounded = std::round(n);

    return {note_number_to_freq(rounded), static_cast<int>(rounded)};
}

std::pair<double, int> FrequencyCalculator::snap_to_scale(const std::vector<int>& scale, double freq) const
{
    auto n = freq_to_note_number(freq);
    auto rounded_n = std::round(n);

    int octave = static_cast<int>(rounded_n) / snapping_N_;
    int n_mod_N = static_cast<int>(rounded_n) % snapping_N_;
    while (n_mod_N < 0) {
        --octave;
        n_mod_N += snapping_N_;
    }

    for (auto i = 0; i < scale.size(); ++i) {
        const auto current = scale[i];
        auto next = i == scale.size() - 1 ? snapping_N_ : scale[i + 1];

        if (current == n_mod_N) {
            break;
        }

        if (current < n_mod_N && n_mod_N < next) {
            auto delta = std::fmod(n, static_cast<double>(snapping_N_));
            while (delta < 0) {
                delta += static_cast<double>(snapping_N_);
            }
            if ((delta - current) <= (next - delta)) {
                rounded_n = static_cast<double>(octave * snapping_N_ + current);
            } else {
                rounded_n = static_cast<double>(octave * snapping_N_ + next);
            }
            break;
        }
    }

    return {note_number_to_freq(rounded_n), static_cast<int>(rounded_n)};
}