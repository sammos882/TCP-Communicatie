#pragma once
#include <omp.h>
#include <iostream>
#include "timerbase.hpp"


class OmpTimer :public TimerBase {
private:
    double _start;
    double _stop;

public:
    OmpTimer();                                // Constructor
    ~OmpTimer();                               // Destructor

    // Rule of Five (optional, trivial here)
    OmpTimer(const OmpTimer& other);
    OmpTimer& operator=(const OmpTimer& other);
    OmpTimer(OmpTimer&& other) noexcept;
    OmpTimer& operator=(OmpTimer&& other) noexcept;

    void startTimer();
    void stopTimer();
    double elapsedTime() const;
};