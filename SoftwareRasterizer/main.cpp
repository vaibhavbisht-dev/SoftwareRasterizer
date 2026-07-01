#include <SDL3/SDL.h>
#include <vector>
#include "math/Vector3.h"
#include "math/Matrix4.h"
#include "math/Utility.h"
#include <cstdint>
#include "FrameBuffer.h"

// Helper structure to define our triangles
struct Triangle {
    int v0, v1, v2;
    uint32_t color;
};

int main(int argc, char* argv[])
{
    const int WIDTH = 800;
    const int HEIGHT = 600;

    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow("Software Rasterizer", WIDTH, HEIGHT, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    SDL_Texture* texture = SDL_CreateTexture(renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        WIDTH, HEIGHT);

    // CPU-side framebuffer
    FrameBuffer framebuffer(WIDTH, HEIGHT);
    framebuffer.SetUsingZBuffer(true);

    // 1. Define 8 cube vertices in object space (±0.5 on each axis)
    std::vector<Vector3<float>> vertices = {
        {-0.5f, -0.5f,  0.5f}, // 0: Front-bottom-left
        { 0.5f, -0.5f,  0.5f}, // 1: Front-bottom-right
        { 0.5f,  0.5f,  0.5f}, // 2: Front-top-right
        {-0.5f,  0.5f,  0.5f}, // 3: Front-top-left
        {-0.5f, -0.5f, -0.5f}, // 4: Back-bottom-left
        { 0.5f, -0.5f, -0.5f}, // 5: Back-bottom-right
        { 0.5f,  0.5f, -0.5f}, // 6: Back-top-right
        {-0.5f,  0.5f, -0.5f}  // 7: Back-top-left
    };

    // 2. Define 12 triangles using CCW winding as seen from outside the cube
    std::vector<Triangle> triangles = {
        // Front face (Red)
        {0, 1, 2, 0xFFFF0000}, {0, 2, 3, 0xFFFF0000},
        // Right face (Green)
        {1, 5, 6, 0xFF00FF00}, {1, 6, 2, 0xFF00FF00},
        // Back face (Blue)
        {5, 4, 7, 0xFF0000FF}, {5, 7, 6, 0xFF0000FF},
        // Left face (Yellow)
        {4, 0, 3, 0xFFFFFF00}, {4, 3, 7, 0xFFFFFF00},
        // Top face (Cyan)
        {3, 2, 6, 0xFF00FFFF}, {3, 6, 7, 0xFF00FFFF},
        // Bottom face (Magenta)
        {4, 5, 1, 0xFFFF00FF}, {4, 1, 0, 0xFFFF00FF}
    };

    // 3. Set view once
    framebuffer.SetViewMatrix(Vector3<float>(0, 0, 3), Vector3<float>(0, 0, 0), Vector3<float>(0, 1, 0));

    // 4. Set projection once
    framebuffer.SetProjectionMatrix(60.0f, (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);

    Vector3<float> Degree(0.0f, 0.0f, 0.0f);

    // Array to hold the transformed vertices for the current frame
    std::vector<Vector3<float>> transformedVertices(8);

    bool running = true;
    SDL_Event event;

    // Use simple SDL ticks for rotation timing
    Uint64 previousTime = SDL_GetTicks();

    while (running)
    {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) running = false;
        }

        Uint64 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - previousTime) / 1000.0f;
        previousTime = currentTime;

        // Clear color and Z-buffer every frame
        framebuffer.clearbuffer();

        // 5a. Increment rotation angle (rotating around X and Y axes)
        Degree.x += 25.0f * deltaTime; // 1 radian per second roughly
        Degree.y += 25.5f * deltaTime;

        // 5b. Call CreateModelMatrix (Center it at origin)
        framebuffer.CreateModelMatrix(Vector3<float>(0, 0, 0), Degree, Vector3<float>(1, 1, 1));

        // 5c. Call ComputeMVPMatrix
        framebuffer.ComputeMVPMatrix();

        // 6a. Transform all 8 vertices
        for (size_t i = 0; i < vertices.size(); i++) {
            transformedVertices[i] = framebuffer.TransformVertex(vertices[i]);
        }

        // 6b. Draw all 12 triangles using the transformed vertices
        for (const auto& tri : triangles) {
            framebuffer.DrawTriangle(
                transformedVertices[tri.v0],
                transformedVertices[tri.v1],
                transformedVertices[tri.v2],
                tri.color
            );
        }

        // Blit framebuffer to screen
        SDL_UpdateTexture(texture, nullptr, framebuffer.getBuffer().data(), WIDTH * sizeof(uint32_t));
        SDL_RenderTexture(renderer, texture, nullptr, nullptr);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}