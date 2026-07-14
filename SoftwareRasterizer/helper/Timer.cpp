#include "Timer.h"

Timer::Timer() {
    QueryPerformanceFrequency(&m_frequency);
    QueryPerformanceCounter(&m_startTime);
    m_lastTickTime = m_startTime;
}

void Timer::Reset() {
    QueryPerformanceCounter(&m_startTime);
    m_lastTickTime = m_startTime;
}

float Timer::Tick() {
    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);

    float deltaTime = static_cast<float>(now.QuadPart - m_lastTickTime.QuadPart) /
        static_cast<float>(m_frequency.QuadPart);
    m_lastTickTime = now;

    // Guard against huge spikes (e.g. breakpoint pause, window drag stall)
    if (deltaTime > 0.25f) deltaTime = 0.25f;

    return deltaTime;
}

float Timer::GetElapsedSeconds() const {
    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);
    return static_cast<float>(now.QuadPart - m_startTime.QuadPart) /
        static_cast<float>(m_frequency.QuadPart);
}