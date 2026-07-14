#include "Win32_Window.h"

Win32_Window::Win32_Window(int width, int height, const std::wstring& title)
    : m_width(width), m_height(height), m_title(title), m_hwnd(nullptr), m_memDC(nullptr), m_dibSection(nullptr), m_dibPixels(nullptr), m_running(false) {}

Win32_Window::~Win32_Window() {
    if (m_dibSection) DeleteObject(m_dibSection);
    if (m_memDC) DeleteDC(m_memDC);
    if (m_hwnd) DestroyWindow(m_hwnd);
}

LRESULT CALLBACK Win32_Window::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    Win32_Window* window = nullptr;

    if (msg == WM_NCCREATE) {
        // Pull the "this" pointer we passed into CreateWindowEx and stash it
        // on the HWND so every later message can find its owning object.
        CREATESTRUCT* cs = reinterpret_cast<CREATESTRUCT*>(lParam);
        window = reinterpret_cast<Win32_Window*>(cs->lpCreateParams);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));
        window->m_hwnd = hwnd; // set early so any handler using m_hwnd is safe
    }
    else {
        window = reinterpret_cast<Win32_Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    }

    if (window) {
        return window->HandleMessage(msg, wParam, lParam);
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

LRESULT Win32_Window::HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_SETCURSOR:
        // Force arrow cursor ourselves so Windows' hang-detection
        // "busy" cursor never overrides it while we're mid-frame.
        if (LOWORD(lParam) == HTCLIENT) {
            SetCursor(LoadCursor(nullptr, IDC_ARROW));
            return TRUE;
        }
        return DefWindowProc(m_hwnd, msg, wParam, lParam);

    case WM_CLOSE:
    case WM_DESTROY:
        m_running = false;
        PostQuitMessage(0);
        return 0;

    default:
        return DefWindowProc(m_hwnd, msg, wParam, lParam);
    }
}

bool Win32_Window::Initialize() {
    HINSTANCE hInstance = GetModuleHandle(nullptr);

    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.lpszClassName = L"SoftwareRasterizerWindowClass";
    if (!RegisterClassEx(&wc)) return false;

    RECT rect = { 0, 0, m_width, m_height };
    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

    m_hwnd = CreateWindowEx(
        0, wc.lpszClassName, m_title.c_str(), WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        rect.right - rect.left, rect.bottom - rect.top,
        nullptr, nullptr, hInstance, this);
    if (!m_hwnd) return false;

    // Set up a DIB section (Device-Independent Bitmap section) matching our framebuffer format (top-down ARGB8888)
    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = m_width;
    bmi.bmiHeader.biHeight = -m_height; // negative = top-down
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    // Create a memory device context and a DIB section for pixel manipulation
    HDC windowDC = GetDC(m_hwnd);
    m_memDC = CreateCompatibleDC(windowDC);
    m_dibSection = CreateDIBSection(windowDC, &bmi, DIB_RGB_COLORS, &m_dibPixels, nullptr, 0);
    ReleaseDC(m_hwnd, windowDC);
    if (!m_dibSection) return false;

    // Select the DIB section into the memory DC so we can draw into it
    SelectObject(m_memDC, m_dibSection);

    ShowWindow(m_hwnd, SW_SHOW);
    UpdateWindow(m_hwnd);
    m_running = true;
    return true;
}

bool Win32_Window::PollEvents() {
    MSG msg;
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT) {
            m_running = false;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return m_running;
}

void Win32_Window::Present(const uint32_t* pixels, int width, int height) {
    // Copy your framebuffer straight into the DIB section's memory
    memcpy(m_dibPixels, pixels, static_cast<size_t>(width) * height * sizeof(uint32_t));

    HDC windowDC = GetDC(m_hwnd);
    BitBlt(windowDC, 0, 0, width, height, m_memDC, 0, 0, SRCCOPY);
    ReleaseDC(m_hwnd, windowDC);
}