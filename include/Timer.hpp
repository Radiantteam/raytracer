#ifndef TIMER_HPP
#define TIMER_HPP

#include <chrono>
#include <string>

class Timer
{
public:
    Timer();

    void Start();

    long long ElapsedMilliseconds() const;
    double ElapsedSeconds() const;

    void PrintElapsed(const std::string& label = "Elapsed time") const;

private:
    std::chrono::high_resolution_clock::time_point m_startTime;
    std::chrono::high_resolution_clock::time_point m_endTime;
    bool m_running;
};

#endif // TIMER_HPP
