#include <iostream>
#include "chronotimer.hpp"
using namespace std::chrono_literals;

// Constructor
ChronoTimer::ChronoTimer(void) {
}

// Destructor
ChronoTimer::~ChronoTimer(void) {
}

// Copy constructor
ChronoTimer::ChronoTimer(const ChronoTimer& other)
    : _start(other._start), _stop(other._stop), _startSysClk(other._startSysClk), _stopSysClk(other._stopSysClk) {
}

// Copy assignment
ChronoTimer& ChronoTimer::operator=(const ChronoTimer& other) {
    if (this != &other) {
        _start = other._start;
        _stop = other._stop;
        _startSysClk = other._startSysClk;
        _stopSysClk = other._stopSysClk;
    }
    return *this;
}

// Move constructor
ChronoTimer::ChronoTimer(ChronoTimer&& other) noexcept
    : _start(std::move(other._start)),
    _stop(std::move(other._stop)),
    _startSysClk(std::move(other._startSysClk)),
    _stopSysClk(std::move(other._stopSysClk)) {
}

// Move assignment
ChronoTimer& ChronoTimer::operator=(ChronoTimer&& other) noexcept {
    if (this != &other) {
        _start = std::move(other._start);
        _stop = std::move(other._stop);
        _startSysClk = std::move(other._startSysClk);
        _stopSysClk = std::move(other._stopSysClk);
    }
    return *this;
}

// Timer functions
void ChronoTimer::startTimer(void) {
    _start = std::chrono::high_resolution_clock::now();
}

void ChronoTimer::stopTimer(void) {
    _stop = std::chrono::high_resolution_clock::now();
}

double ChronoTimer::elapsedTime(void) const {
    std::chrono::duration<double> duration = _stop - _start;
    return duration.count();
}

void ChronoTimer::startTimerSystemClock(void) {
    _startSysClk = std::chrono::system_clock::now();
}

void ChronoTimer::stopTimerSystemClock(void) {
    _stopSysClk = std::chrono::system_clock::now();
}

double ChronoTimer::elapsedTimeSystemClock(void) const {
    std::chrono::duration<double> diff = _stopSysClk - _startSysClk;
    return diff.count();
}
