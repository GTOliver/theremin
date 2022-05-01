#pragma once

class FrequencyCalculator
{
public:
    double calculate(double position);

private:
    double range_{0.5};
    double min_freq_{500};
    double max_freq_{2000};
};
