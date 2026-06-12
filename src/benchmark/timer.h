#pragma once
#include <chrono>

class Timer {
    using Clock = std::chrono::high_resolution_clock;
    using TimePoint = std::chrono::time_point<Clock>;

    TimePoint startTime;
public:
    void start() { startTime = Clock::now(); }
    double elapsedMs() const {
        auto now = Clock::now();
        return std::chrono::duration<double, std::milli>(now - startTime).count();
    }
};
