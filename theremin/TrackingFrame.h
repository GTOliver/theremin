#pragma once

#include <optional>

struct HandData
{
    double x;
    double y;
    double z;
};

struct TrackingFrame
{
    TrackingFrame()
            : timestamp{0},
              left{std::nullopt},
              right{std::nullopt}
    {}

    int64_t timestamp;
    std::optional<HandData> left;
    std::optional<HandData> right;
};
