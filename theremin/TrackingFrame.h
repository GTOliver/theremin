#pragma once

#include <optional>

struct Vector3
{
    double x;
    double y;
    double z;
};

struct HandData
{
    Vector3 palm_position;
    Vector3 average_finger_position;
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
