#ifndef _TIMERBASE_HPP_
#define _TIMERBASE_HPP_

class TimerBase {
public:
    virtual ~TimerBase() = default; // altijd virtuele destructor bij polymorfisme

    virtual void startTimer() = 0;        // pure virtual
    virtual void stopTimer() = 0;         // pure virtual
    virtual double elapsedTime() const = 0; // pure virtual
};
#endif
