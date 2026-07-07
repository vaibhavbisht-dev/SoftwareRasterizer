#include <SDL3/SDL.h>
#include <vector>
#include "math/Vector3.h"
#include "math/Matrix4.h"
#include "math/Utility.h"
#include <cstdint>
#include "FrameBuffer.h"
#include "helper/SoftwareTexture.h"
#include "helper/Parser.h"

// Helper structure to define our triangles


int main(int argc, char* argv[])
{
    const int WIDTH = 1920;
    const int HEIGHT = 1080;

    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow("Software Rasterizer", WIDTH, HEIGHT, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    SDL_Texture* texture = SDL_CreateTexture(renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        WIDTH, HEIGHT);

	Vector3<float> CameraPos(0.0f, 2.0f, 7.0f);
    // CPU-side framebuffer
    FrameBuffer framebuffer(WIDTH, HEIGHT);
    framebuffer.SetUsingZBuffer(true);
	framebuffer.SetLightSource(Vector3<float>(0.0f, 5.0f, 8.0f)); // Light coming from the camera direction
	framebuffer.SetCameraPosition(CameraPos);

	//SoftwareTexture Mytexture("./assets/texture.jpg"); // Load your texture here
	OBJParser parser("./assets/car.obj");
	SoftwareTexture Mytexture("./assets/car_texture.png"); // Load your texture here

	ObjectData objData = parser.ParseOBJ();
	std::vector<Vertex> vertices = objData.vertices;
    std::vector<Triangle> triangles = objData.triangles;



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

        // 5a. Increment rotation angle (rotating around Y axis)
        Degree.y += 25.5f * deltaTime;// 1 radian per second roughly

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