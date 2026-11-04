#include "linuxtimer.hpp"
#include <iostream>

// Constructor
LinuxTimer::LinuxTimer(void) {
    _start = {0,0};
    _stop = {0,0};
}

// Destructor
LinuxTimer::~LinuxTimer(void) {}

// Copy constructor
LinuxTimer::LinuxTimer(const LinuxTimer& other)
    : _start(other._start), _stop(other._stop) {}

// Copy assignment
LinuxTimer& LinuxTimer::operator=(const LinuxTimer& other) {
    if (this != &other) {
        _start = other._start;
        _stop = other._stop;
    }
    return *this;
}

// Move constructor
LinuxTimer::LinuxTimer(LinuxTimer&& other) noexcept
    : _start(other._start), _stop(other._stop) {
    other._start = {0,0};
    other._stop = {0,0};
}

// Move assignment
LinuxTimer& LinuxTimer::operator=(LinuxTimer&& other) noexcept {
    if (this != &other) {
        _start = other._start;
        _stop = other._stop;
        other._start = {0,0};
        other._stop = {0,0};
    }
    return *this;
}

// Timer functions
void LinuxTimer::startTimer() {
    clock_gettime(CLOCK_MONOTONIC_RAW, &_start);
}

void LinuxTimer::stopTimer() {
    clock_gettime(CLOCK_MONOTONIC_RAW, &_stop);
}

double LinuxTimer::elapsedTime() const {
    double start_sec = static_cast<double>(_start.tv_sec) + _start.tv_nsec / 1e9;
    double stop_sec  = static_cast<double>(_stop.tv_sec)  + _stop.tv_nsec  / 1e9;
    return stop_sec - start_sec;
}
