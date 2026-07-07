#include <SDL3/SDL.h>
#include <vector>
#include "math/Vector3.h"
#include "math/Matrix4.h"
#include "math/Utility.h"
#include <cstdint>
#include "FrameBuffer.h"
#include "helper/SoftwareTexture.h"

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

	Vector3<float> CameraPos(0.0f, 0.0f, 3.0f);
    // CPU-side framebuffer
    FrameBuffer framebuffer(WIDTH, HEIGHT);
    framebuffer.SetUsingZBuffer(true);
	framebuffer.SetLightSource(Vector3<float>(0.0f, 5.0f, 8.0f)); // Light coming from the camera direction
	framebuffer.SetCameraPosition(CameraPos);
	//SoftwareTexture Mytexture("./assets/texture.jpg"); // Load your texture here
	SoftwareTexture Mytexture("./assets/texture1.png"); // Load your texture here

    std::vector<Vertex> vertices = {
        // --- Front Face (Z = 0.5f) | Normal: (0, 0, 1) ---
        {Vector3<float>(-0.5f, -0.5f,  0.5f), Vector2<float>(0.0f, 0.0f), Vector3<float>(0.0f, 0.0f, 1.0f)}, // 0
        {Vector3<float>(0.5f, -0.5f,  0.5f), Vector2<float>(1.0f, 0.0f), Vector3<float>(0.0f, 0.0f, 1.0f)}, // 1
        {Vector3<float>(0.5f,  0.5f,  0.5f), Vector2<float>(1.0f, 1.0f), Vector3<float>(0.0f, 0.0f, 1.0f)}, // 2
        {Vector3<float>(-0.5f,  0.5f,  0.5f), Vector2<float>(0.0f, 1.0f), Vector3<float>(0.0f, 0.0f, 1.0f)}, // 3

        // --- Back Face (Z = -0.5f) | Normal: (0, 0, -1) ---
        {Vector3<float>(0.5f, -0.5f, -0.5f), Vector2<float>(0.0f, 0.0f), Vector3<float>(0.0f, 0.0f, -1.0f)}, // 4
        {Vector3<float>(-0.5f, -0.5f, -0.5f), Vector2<float>(1.0f, 0.0f), Vector3<float>(0.0f, 0.0f, -1.0f)}, // 5
        {Vector3<float>(-0.5f,  0.5f, -0.5f), Vector2<float>(1.0f, 1.0f), Vector3<float>(0.0f, 0.0f, -1.0f)}, // 6
        {Vector3<float>(0.5f,  0.5f, -0.5f), Vector2<float>(0.0f, 1.0f), Vector3<float>(0.0f, 0.0f, -1.0f)}, // 7

        // --- Top Face (Y = 0.5f) | Normal: (0, 1, 0) ---
        {Vector3<float>(-0.5f,  0.5f,  0.5f), Vector2<float>(0.0f, 0.0f), Vector3<float>(0.0f, 1.0f, 0.0f)}, // 8
        {Vector3<float>(0.5f,  0.5f,  0.5f), Vector2<float>(1.0f, 0.0f), Vector3<float>(0.0f, 1.0f, 0.0f)}, // 9
        {Vector3<float>(0.5f,  0.5f, -0.5f), Vector2<float>(1.0f, 1.0f), Vector3<float>(0.0f, 1.0f, 0.0f)}, // 10
        {Vector3<float>(-0.5f,  0.5f, -0.5f), Vector2<float>(0.0f, 1.0f), Vector3<float>(0.0f, 1.0f, 0.0f)}, // 11

        // --- Bottom Face (Y = -0.5f) | Normal: (0, -1, 0) ---
        {Vector3<float>(-0.5f, -0.5f, -0.5f), Vector2<float>(0.0f, 0.0f), Vector3<float>(0.0f, -1.0f, 0.0f)}, // 12
        {Vector3<float>(0.5f, -0.5f, -0.5f), Vector2<float>(1.0f, 0.0f), Vector3<float>(0.0f, -1.0f, 0.0f)}, // 13
        {Vector3<float>(0.5f, -0.5f,  0.5f), Vector2<float>(1.0f, 1.0f), Vector3<float>(0.0f, -1.0f, 0.0f)}, // 14
        {Vector3<float>(-0.5f, -0.5f,  0.5f), Vector2<float>(0.0f, 1.0f), Vector3<float>(0.0f, -1.0f, 0.0f)}, // 15

        // --- Right Face (X = 0.5f) | Normal: (1, 0, 0) ---
        {Vector3<float>(0.5f, -0.5f,  0.5f), Vector2<float>(0.0f, 0.0f), Vector3<float>(1.0f, 0.0f, 0.0f)}, // 16
        {Vector3<float>(0.5f, -0.5f, -0.5f), Vector2<float>(1.0f, 0.0f), Vector3<float>(1.0f, 0.0f, 0.0f)}, // 17
        {Vector3<float>(0.5f,  0.5f, -0.5f), Vector2<float>(1.0f, 1.0f), Vector3<float>(1.0f, 0.0f, 0.0f)}, // 18
        {Vector3<float>(0.5f,  0.5f,  0.5f), Vector2<float>(0.0f, 1.0f), Vector3<float>(1.0f, 0.0f, 0.0f)}, // 19

        // --- Left Face (X = -0.5f) | Normal: (-1, 0, 0) ---
        {Vector3<float>(-0.5f, -0.5f, -0.5f), Vector2<float>(0.0f, 0.0f), Vector3<float>(-1.0f, 0.0f, 0.0f)}, // 20
        {Vector3<float>(-0.5f, -0.5f,  0.5f), Vector2<float>(1.0f, 0.0f), Vector3<float>(-1.0f, 0.0f, 0.0f)}, // 21
        {Vector3<float>(-0.5f,  0.5f,  0.5f), Vector2<float>(1.0f, 1.0f), Vector3<float>(-1.0f, 0.0f, 0.0f)}, // 22
        {Vector3<float>(-0.5f,  0.5f, -0.5f), Vector2<float>(0.0f, 1.0f), Vector3<float>(-1.0f, 0.0f, 0.0f)}  // 23
    };


    // 2. Updated 12 triangles using Counter-Clockwise (CCW) winding referencing the unique vertices
    std::vector<Triangle> triangles = {
        // Front face (Pure texture colors)
        {0, 1, 2, 0xFFFFFFFF}, {0, 2, 3, 0xFFFFFFFF},
        // Back face 
        {4, 5, 6, 0xFFFFFFFF}, {4, 6, 7, 0xFFFFFFFF},
        // Top face
        {8, 9, 10, 0xFFFFFFFF}, {8, 10, 11, 0xFFFFFFFF},
        // Bottom face
        {12, 13, 14, 0xFFFFFFFF}, {12, 14, 15, 0xFFFFFFFF},
        // Right face
        {16, 17, 18, 0xFFFFFFFF}, {16, 18, 19, 0xFFFFFFFF},
        // Left face
        {20, 21, 22, 0xFFFFFFFF}, {20, 22, 23, 0xFFFFFFFF}
    };

    // 3. Set view once

    // 4. Set projection once
    framebuffer.SetProjectionMatrix(60.0f, (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);

    Vector3<float> Degree(0.0f, 0.0f, 0.0f);

    // Array to hold the transformed vertices for the current frame
    std::vector<TransformedVertex> transformedVertices(vertices.size());

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
        framebuffer.SetViewMatrix(CameraPos, Vector3<float>(0, 0, 0), Vector3<float>(0, 1, 0));

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
                Mytexture
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