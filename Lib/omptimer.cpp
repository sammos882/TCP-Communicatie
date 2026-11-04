#include "omptimer.hpp"

// Constructor
OmpTimer::OmpTimer() : _start(0.0), _stop(0.0) {
}

// Destructor
OmpTimer::~OmpTimer() {
}

// Copy constructor
OmpTimer::OmpTimer(const OmpTimer& other)
    : _start(other._start), _stop(other._stop) {
}

// Copy assignment
OmpTimer& OmpTimer::operator=(const OmpTimer& other) {
    if (this != &other) {
        _start = other._start;
        _stop = other._stop;
    }
    return *this;
}

// Move constructor
OmpTimer::OmpTimer(OmpTimer&& other) noexcept
    : _start(other._start), _stop(other._stop) {
    other._start = 0.0;
    other._stop = 0.0;
}

// Move assignment
OmpTimer& OmpTimer::operator=(OmpTimer&& other) noexcept {
    if (this != &other) {
        _start = other._start;
        _stop = other._stop;
        other._start = 0.0;
        other._stop = 0.0;
    }
    return *this;
}

// Start timer
void OmpTimer::startTimer() {
    _start = omp_get_wtime();
}

// Stop timer
void OmpTimer::stopTimer() {
    _stop = omp_get_wtime();
}

// Elapsed seconds
double OmpTimer::elapsedTime() const {
    return _stop - _start;
}