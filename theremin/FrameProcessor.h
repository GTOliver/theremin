#pragma once

#include <atomic>
#include <optional>
#include <utility>

#include "Bounds.h"

#include "ThereMessage.h"
#include "TrackingFrame.h"

#include "FrequencyCalculator.h"
#include "LevelCalculator.h"

class FrameProcessor
{
public:
    std::optional<ThereMessage> process(const TrackingFrame& frame);

    void set_level_physical_bounds(Bounds bounds);

    void set_level_bounds(Bounds bounds);

    void set_frequency_physical_range(double range);

    void set_frequency_output_bounds(Bounds bounds);

    void set_frequency_scaling(ScalingMethod scaling);

    void set_snapping_mode(SnappingMode mode);

    void set_use_fingers_enabled(bool enabled);

private:
    std::optional<ThereMessage> switch_off();

    bool currently_emitting_{false};
    std::atomic<bool> use_fingers_{false};

    LevelCalculator level_calculator_;
    FrequencyCalculator frequency_calculator_;
};
