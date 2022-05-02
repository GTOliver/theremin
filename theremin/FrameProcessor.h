#pragma once

#include <optional>
#include <utility>

#include "Bounds.h"

#include "TrackingFrame.h"
#include "ThereMessage.h"

#include "LevelCalculator.h"
#include "FrequencyCalculator.h"

class FrameProcessor
{
public:
    std::optional<ThereMessage> process(const TrackingFrame& frame);

    void set_level_physical_bounds(Bounds bounds);

    void set_max_level(double max);

    void set_frequency_physical_range(double range);

    void set_frequency_output_bounds(Bounds bounds);

    void set_frequency_scaling(ScalingMethod scaling);

    void set_snapping_enabled(bool enabled);

private:
    bool currently_emitting_{false};

    LevelCalculator level_calculator_;
    FrequencyCalculator frequency_calculator_;
};
