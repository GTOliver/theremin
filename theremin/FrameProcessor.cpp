#include "FrameProcessor.h"

std::optional<ThereMessage> FrameProcessor::process(const TrackingFrame& frame)
{
    if (!(frame.right.has_value() && frame.left.has_value())) {
        auto retval = currently_emitting_ ? std::optional<ThereMessage>({0.0, 0.0}) : std::nullopt;
        currently_emitting_ = false;
        return retval;
    }

    currently_emitting_ = true;

    double mm_to_m = 0.001;
    double level = level_calculator_.calculate(frame.left->y * mm_to_m);
    double frequency = frequency_calculator_.calculate(frame.right->z * mm_to_m * -1.0);

    return ThereMessage{level, frequency};
}

void FrameProcessor::set_level_physical_bounds(Bounds bounds)
{
    level_calculator_.set_distance_bounds(std::move(bounds));
}

void FrameProcessor::set_max_level(double max)
{
    level_calculator_.set_max_level(max);
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
