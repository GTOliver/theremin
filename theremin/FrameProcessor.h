#pragma once

#include <optional>

#include "TrackingFrame.h"
#include "ThereMessage.h"

#include "LevelCalculator.h"
#include "FrequencyCalculator.h"

class FrameProcessor
{
public:
    std::optional<ThereMessage> process(const TrackingFrame& frame);

private:
    bool currently_emitting_{false};

    LevelCalculator level_calculator_;
    FrequencyCalculator frequency_calculator_;
};
