#pragma once
#include <Windows.h>
#include <cstdint>
#include <string>

class Win32_Window {
public:
    Win32_Window(int width, int height, const std::wstring& title);
    ~Win32_Window();

    bool Initialize();
    void SetTitle(const char* title);
    // Pumps the Windows message queue. Returns false when the window should close.
    bool PollEvents();

    // Blits an ARGB8888 pixel buffer to the client area.
    void Present(const uint32_t* pixels, int width, int height);

    HWND GetHandle() const { return m_hwnd; }
    bool IsRunning() const { return m_running; }

private:
    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    LRESULT HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam);

    int m_width;
    int m_height;
    std::wstring m_title;

    HWND m_hwnd;
    HDC m_memDC;          // memory DC for the DIB section
    HBITMAP m_dibSection; // backing store we blit into
    void* m_dibPixels;    // pointer to the DIB's pixel memory
    bool m_running;
};