#pragma once

// Theremin-message
struct ThereMessage
{
    bool off{false};
    bool note_change{false};
    double level{0.0};
    double frequency{0.0};
    double cc{0.0};
};
