#pragma once

#include <chrono>

struct FpsCounter
{
    std::chrono::time_point<std::chrono::steady_clock> last_time;
    double deltas[32];
    int i;
    int count;

    void update();
    double get();
};
