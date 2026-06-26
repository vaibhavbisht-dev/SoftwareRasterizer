#include <SDL3/SDL.h>
#include <vector>
#include "math/Vector3.h"
#include <cstdint>
#include "FrameBuffer.h"

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

    // Your CPU-side framebuffer — this is what you'll draw into
	FrameBuffer framebuffer(WIDTH, HEIGHT);
	framebuffer.SetUsingZBuffer(true);

    bool running = true;
    SDL_Event event;

    while (running)
    {
        while (SDL_PollEvent(&event))
            if (event.type == SDL_EVENT_QUIT) running = false;

		framebuffer.clearbuffer();

        

        // 2. Top-Center Green Inner Triangle
        framebuffer.DrawTriangle(Vector3<float>(400, 69, 0), Vector3<float>(300, 242, 0), Vector3<float>(500, 242, 0), 0xFF00FF00);

        // 3. Bottom-Left Blue Inner Triangle
        framebuffer.DrawTriangle(Vector3<float>(300, 242, 0), Vector3<float>(200, 415, 0), Vector3<float>(400, 415, 0), 0xFF0000FF);

        // 4. Bottom-Right Yellow Inner Triangle
        framebuffer.DrawTriangle(Vector3<float>(500, 242, 0), Vector3<float>(400, 415, 0), Vector3<float>(600, 415, 0), 0xFFFFFF00);
        
        
        // 1. Large Red Outer Triangle
        framebuffer.DrawTriangle(Vector3<float>(400, 69, 10), Vector3<float>(200, 415, 10), Vector3<float>(600, 415, 10), 0xFFFF0000);

        
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