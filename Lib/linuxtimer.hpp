#ifndef _LINUXTIMER_HPP_
#define _LINUXTIMER_HPP_

#include <time.h>
#include "timerbase.hpp"

// Linux only, high precision timer using clock_gettime
class LinuxTimer : public TimerBase {
private:
    struct timespec _start;
    struct timespec _stop;

public:
    LinuxTimer(void);                                  // Constructor
    ~LinuxTimer(void);                                 // Destructor
    // Rule of Five
    LinuxTimer(const LinuxTimer& other);               // Copy constructor
    LinuxTimer& operator=(const LinuxTimer& other);    // Copy assignment
    LinuxTimer(LinuxTimer&& other) noexcept;           // Move constructor
    LinuxTimer& operator=(LinuxTimer&& other) noexcept;// Move assignment

    void startTimer();
    void stopTimer();
    double elapsedTime() const;
};

#endif
