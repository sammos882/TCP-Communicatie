#include "WindowsTimer.hpp"
#include <iostream>

// Constructor
WindowsTimer::WindowsTimer(void) {

    QueryPerformanceFrequency(&_frequency);
    _start.QuadPart = 0;
    _stop.QuadPart = 0;
}

// Destructor
WindowsTimer::~WindowsTimer(void) {
}

// Copy constructor
WindowsTimer::WindowsTimer(const WindowsTimer& other)
    : _frequency(other._frequency), _start(other._start), _stop(other._stop) {
}

// Copy assignment
WindowsTimer& WindowsTimer::operator=(const WindowsTimer& other) {
    if (this != &other) {
        _frequency = other._frequency;
        _start = other._start;
        _stop = other._stop;
    }
    return *this;
}

// Move constructor
WindowsTimer::WindowsTimer(WindowsTimer&& other) noexcept
    : _frequency(other._frequency), _start(other._start), _stop(other._stop) {
    // niet echt nodig, maar je zou "other" kunnen resetten
    other._frequency.QuadPart = 0;
    other._start.QuadPart = 0;
    other._stop.QuadPart = 0;
}

// Move assignment
WindowsTimer& WindowsTimer::operator=(WindowsTimer&& other) noexcept {
    if (this != &other) {
        _frequency = other._frequency;
        _start = other._start;
        _stop = other._stop;

        other._frequency.QuadPart = 0;
        other._start.QuadPart = 0;
        other._stop.QuadPart = 0;
    }
    return *this;
}

// Timer functions
void WindowsTimer::startTimer() {
    QueryPerformanceCounter(&_start);
}

void WindowsTimer::stopTimer() {
    QueryPerformanceCounter(&_stop);
}

double WindowsTimer::elapsedTime() const {
    return static_cast<double>(_stop.QuadPart - _start.QuadPart) / _frequency.QuadPart;
}
