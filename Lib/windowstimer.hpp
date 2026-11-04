#ifndef _WINDOWSTIMER_HPP_
#define _WINDOWSTIMER_HPP_

#include <windows.h>
#include "timerbase.hpp"

//Windows only, max precision
class WindowsTimer :public TimerBase {
private:
    LARGE_INTEGER _frequency;
    LARGE_INTEGER _start;
    LARGE_INTEGER _stop;

public:
    WindowsTimer(void);                                // Constructor
    ~WindowsTimer(void);                               // Destructor
    // Rule of Five
    WindowsTimer(const WindowsTimer& other);               // Copy constructor
    WindowsTimer& operator=(const WindowsTimer& other);    // Copy assignment
    WindowsTimer(WindowsTimer&& other) noexcept;           // Move constructor
    WindowsTimer& operator=(WindowsTimer&& other) noexcept;// Move assignment

    void startTimer();
    void stopTimer();
    double elapsedTime() const;
};
#endif
