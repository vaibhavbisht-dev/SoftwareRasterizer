//#include <SDL3/SDL.h>
#include <vector>
#include <cstdint>
#include "Renderer/Renderer.h"
#include "FrameBuffer.h"
#include "helper/SoftwareTexture.h"
#include "helper/Parser.h"
#include "Window/Win32_Window.h"
#include "helper/Timer.h"

int main(int argc, char* argv[]) {
    // Must be called before creating any window. Tells Windows we handle
    // scaling ourselves, so sizes we pass to CreateWindowEx are real
    // physical pixels — no automatic DWM upscaling of the window.
    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    const int WIDTH = 1920;
    const int HEIGHT = 1080;

    Win32_Window window(WIDTH, HEIGHT, L"Software Rasterizer");
    if (!window.Initialize()) {
        std::cerr << "Failed to initialize window." << std::endl;
        return -1;
    }

    // Instantiate CPU pixel canvas context
    FrameBuffer framebuffer(WIDTH, HEIGHT);

    // Pass canvas reference to manage pipeline operations separately
    Renderer renderer(WIDTH, HEIGHT, framebuffer, window);
    if (!renderer.Initialize()) {
        std::cerr << "Failed to initialize renderer." << std::endl;
        return -1;
    }

    // Load geometry assets outside window updates
    OBJParser parser("./assets/car.obj");
    SoftwareTexture myTexture("./assets/car_texture.png");

    ObjectData objData = parser.ParseOBJ();
    std::vector<Vertex> vertices = objData.vertices;
    std::vector<Triangle> triangles = objData.triangles;

    bool running = true;
    Timer timer;

    // Polling and application main execution loop
    while (running) {
        // Pump Windows messages every frame — without this the window
        // never processes input/close/paint and gets marked "not responding".
        running = window.PollEvents();
        if (!running) break;

        float deltaTime = timer.Tick();

        // Delegate entire scene transformation and rasterization pipeline
        renderer.RenderFrame(vertices, triangles, myTexture, deltaTime);
    }

    return 0;
}