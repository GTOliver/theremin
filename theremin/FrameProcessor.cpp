#include "FrameProcessor.h"

std::optional<ThereMessage> FrameProcessor::process(const TrackingFrame& frame)
{
    if (!(frame.right.has_value() && frame.left.has_value())) {
        return switch_off();
    }

    const auto get_position = [this](const auto& hand) {
        return use_fingers_.load() ? hand.average_finger_position : hand.palm_position;
    };

    const auto left_position = get_position(frame.left.value());

    double mm_to_m = 0.001;
    double level = level_calculator_.calculate(left_position.y * mm_to_m);

    if (level == 0.0) {
        return switch_off();
    }

    currently_emitting_ = true;

    const auto right_position = get_position(frame.right.value());

    auto [frequency, note_changed] = frequency_calculator_.calculate(right_position.z * mm_to_m * -1.0);

    return ThereMessage{false, note_changed, level, frequency, frame.left->grab_strength};
}

void FrameProcessor::set_level_physical_bounds(Bounds bounds)
{
    level_calculator_.set_distance_bounds(std::move(bounds));
}

void FrameProcessor::set_level_bounds(Bounds bounds)
{
    level_calculator_.set_level_bounds(bounds);
}

void FrameProcessor::set_frequency_physical_range(double range)
{
    frequency_calculator_.set_distance_range(range);
}

void FrameProcessor::set_frequency_output_bounds(Bounds bounds)
{
    frequency_calculator_.set_frequency_bounds(std::move(bounds));
}

void FrameProcessor::set_frequency_scaling(ScalingMethod scaling)
{
    frequency_calculator_.set_scaling_method(scaling);
}

void FrameProcessor::set_snapping_mode(SnappingMode mode)
{
    frequency_calculator_.set_snapping_mode(mode);
}

void FrameProcessor::set_use_fingers_enabled(bool enabled)
{
    use_fingers_.store(enabled);
}

std::optional<ThereMessage> FrameProcessor::switch_off()
{
    const auto retval = currently_emitting_ ? std::optional<ThereMessage>({true}) : std::nullopt;
    currently_emitting_ = false;
    return retval;
}
