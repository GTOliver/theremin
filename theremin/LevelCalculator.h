#pragma once

class LevelCalculator
{
public:
    [[nodiscard]] double calculate(double distance) const;

private:
    double max_volume_ = 0.2;
    double min_distance_ = 0.15;
    double max_distance_ = 0.5;
};
