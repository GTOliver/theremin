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
