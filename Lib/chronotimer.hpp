#ifndef _CHRONOTIMER_HPP_
#define _CHRONOTIMER_HPP_

#include <chrono>
#include "timerbase.hpp"

//portable, C++11+	std::chrono::high_resolution_clock or steady_clock
class ChronoTimer :public TimerBase {
private:
    // Precision: Higher - uses std::chrono::high_resolution_clock
    std::chrono::high_resolution_clock::time_point _start;
    std::chrono::high_resolution_clock::time_point _stop;
    // Precision: Lower - uses std::chrono::system_clock
    std::chrono::time_point<std::chrono::system_clock> _startSysClk;
    std::chrono::time_point<std::chrono::system_clock> _stopSysClk;
public:
    ChronoTimer(void);                                      // Constructor
    ~ChronoTimer(void);                                     // Rule of Three/Five : 1.Destructor
    ChronoTimer(const ChronoTimer& other);                  // Rule of Three/Five : 2.Copy constructor
    ChronoTimer& operator=(const ChronoTimer& other);       // Rule of Three/Five : 3.Copy assignment
    ChronoTimer(ChronoTimer&& other) noexcept;              // Rule of Three/Five : 4.Move constructor
    ChronoTimer& operator=(ChronoTimer&& other) noexcept;   // Rule of Three/Five : 5.Move assignment

    void startTimer(void);
    void stopTimer(void);
    double elapsedTime(void) const;

    void startTimerSystemClock(void);
    void stopTimerSystemClock(void);
    double elapsedTimeSystemClock(void) const;
};

#endif
