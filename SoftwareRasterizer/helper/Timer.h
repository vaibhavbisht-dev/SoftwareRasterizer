#pragma once
#include <Windows.h>
#include <cstdint>

class Timer {
public:
    Timer();

    // Call once per frame. Returns delta time in seconds since the last call.
    float Tick();

    // Time in seconds since the Timer was constructed (or since last Reset()).
    float GetElapsedSeconds() const;

    void Reset();

private:
    LARGE_INTEGER m_frequency;
    LARGE_INTEGER m_startTime;
    LARGE_INTEGER m_lastTickTime;
};