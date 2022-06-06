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
            freq_and_note_number = snap_to_chromatic(12, freq);
            break;
        case (SnappingMode::Chromatic17):
            freq_and_note_number = snap_to_chromatic(17, freq);
            break;
        case (SnappingMode::Pentatonic):
            freq_and_note_number = snap_to_scale(12, pentatonic_12_, freq);
            break;
        case (SnappingMode::Major):
            freq_and_note_number = snap_to_scale(12, major_12_, freq);
            break;
        case (SnappingMode::NoneWithChromaticEnvelope):
            freq_and_note_number = {freq, snap_to_chromatic(12, freq).second};
            break;
        case (SnappingMode::Major17):
            freq_and_note_number = snap_to_scale(17, major_17_, freq);
            break;
        case (SnappingMode::Pentatonic17):
            freq_and_note_number = snap_to_scale(17, pentatonic_17_, freq);
            break;
        case (SnappingMode::Pentatonic11):
            freq_and_note_number = snap_to_scale(11, pentatonic_11_, freq);
            break;
        case (SnappingMode::Pentatonic14):
            freq_and_note_number = snap_to_scale(14, pentatonic_14_, freq);
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

std::vector<int> FrequencyCalculator::calculate_major_scale(int large, int small)
{
    return {0,
            large,
            2 * large,
            2 * large + small,
            3 * large + small,
            4 * large + small,
            5 * large + small};
}

std::vector<int> FrequencyCalculator::calculate_pentatonic_major_scale(int large, int small)
{
    return {0, small, 2 * small, 2 * small + large, 3 * small + large};
}

std::pair<double, int> FrequencyCalculator::snap_to_scale(int n_notes, const std::vector<int>& scale, double freq) const
{
    {
        auto n = freq_to_note_number(n_notes, freq);
        auto rounded_n = std::round(n);

        int octave = static_cast<int>(rounded_n) / n_notes;
        int n_mod_N = static_cast<int>(rounded_n) % n_notes;
        while (n_mod_N < 0) {
            --octave;
            n_mod_N += n_notes;
        }

        for (auto i = 0; i < scale.size(); ++i) {
            const auto current = scale[i];
            auto next = i == scale.size() - 1 ? n_notes : scale[i + 1];

            if (current == n_mod_N) {
                break;
            }

            if (current < n_mod_N && n_mod_N < next) {
                auto delta = std::fmod(n, static_cast<double>(n_notes));
                while (delta < 0) {
                    delta += static_cast<double>(n_notes);
                }
                if ((delta - current) <= (next - delta)) {
                    rounded_n = static_cast<double>(octave * n_notes + current);
                } else {
                    rounded_n = static_cast<double>(octave * n_notes + next);
                }
                break;
            }
        }

        return {note_number_to_freq(n_notes, rounded_n), static_cast<int>(rounded_n)};
    }
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
            double frequency_range = frequency_.max() - frequency_.min();
            double distance_scaling = std::sqrt(1.0 - progress);

            return frequency_.max() - frequency_range * distance_scaling;
        }
        default:
            // This should never happen
            return frequency_.min();
    }
}

double FrequencyCalculator::freq_to_note_number(int n_notes, double freq) const
{
    return static_cast<double>(n_notes) * std::log2(freq / snapping_f0_);
}

double FrequencyCalculator::note_number_to_freq(int n_notes, double n) const
{
    return snapping_f0_ * std::pow(2.0, n / static_cast<double>(n_notes));
}

std::pair<double, int> FrequencyCalculator::snap_to_chromatic(int n_notes, double freq) const
{
    auto n = freq_to_note_number(n_notes, freq);
    auto rounded = std::round(n);

    return {note_number_to_freq(n_notes, rounded), static_cast<int>(rounded)};
}
