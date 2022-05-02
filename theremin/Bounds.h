#pragma once

#include <utility>

// Simple wrapper around std::pair
struct Bounds
{
public:
    template<class... Args>
    explicit Bounds(Args... args)
            : pair_(args...)
    {}

    [[nodiscard]] const double& min() const
    {
        return pair_.first;
    }

    [[nodiscard]] const double& max() const
    {
        return pair_.second;
    }

private:
    std::pair<double, double> pair_;
};
