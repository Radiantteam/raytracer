#include "Timer.hpp"
#include <iostream>

Timer::Timer() : m_running(false)
{
    Start();
}

void Timer::Start()
{
    m_startTime = std::chrono::high_resolution_clock::now();
    m_running = true;
}

long long Timer::ElapsedMilliseconds() const
{
    auto endTime = m_running ? std::chrono::high_resolution_clock::now() : m_endTime;
    return std::chrono::duration_cast<std::chrono::milliseconds>(endTime - m_startTime).count();
}

double Timer::ElapsedSeconds() const
{
    return ElapsedMilliseconds() / 1000.0;
}

void Timer::PrintElapsed(const std::string& label) const
{
    auto ms = ElapsedMilliseconds();
    std::cout << label << ": " << ms << " ms (" << ms / 1000.0 << " seconds)" << std::endl;
}
