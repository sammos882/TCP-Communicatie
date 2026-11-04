#ifndef _CHRONOTIMER_HPP_
#define _CHRONOTIMER_HPP_
#include <time.h>
#include "timerbase.hpp"

//portable, C++11+	std::chrono::system_clock or steady_clock
class clocktimer :public TimerBase {
private:
    clock_t _start;
    clock_t _stop;
public:
    clocktimer(void);                                      // Constructor
    ~clocktimer(void);                                     // Rule of Three/Five : 1.Destructor
    clocktimer(const clocktimer& other);                  // Rule of Three/Five : 2.Copy constructor
    clocktimer& operator=(const clocktimer& other);       // Rule of Three/Five : 3.Copy assignment
    clocktimer(clocktimer&& other) noexcept;              // Rule of Three/Five : 4.Move constructor
    clocktimer& operator=(clocktimer&& other) noexcept;   // Rule of Three/Five : 5.Move assignment

    void startTimer(void);
    void stopTimer(void);
    double elapsedTime(void) const;
};

#endif
