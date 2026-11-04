#include <iostream>
#include "clocktimer.hpp"

// Constructor
clocktimer::clocktimer(void) {
}

// Destructor
clocktimer::~clocktimer(void) {
}

// Copy constructor
clocktimer::clocktimer(const clocktimer& other)
    : _start(other._start), _stop(other._stop) {
}

// Copy assignment
clocktimer& clocktimer::operator=(const clocktimer& other) {
    if (this != &other) {
        _start = other._start;
        _stop = other._stop;
    }
    return *this;
}

// Move constructor
clocktimer::clocktimer(clocktimer&& other) noexcept
    : _start(std::move(other._start)),
    _stop(std::move(other._stop)) {
}

// Move assignment
clocktimer& clocktimer::operator=(clocktimer&& other) noexcept {
    if (this != &other) {
        _start = std::move(other._start);
        _stop = std::move(other._stop);
    }
    return *this;
}

// Timer functions
void clocktimer::startTimer(void) {
    _start = clock();
}

void clocktimer::stopTimer(void) {
    _stop = clock();
}

double clocktimer::elapsedTime(void) const {
	clock_t duration = _stop - _start;
    return ((double)duration) / CLOCKS_PER_SEC;
}
