#include <SDL3/SDL.h>
#include <vector>
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

    bool running = true;
    SDL_Event event;

    while (running)
    {
        while (SDL_PollEvent(&event))
            if (event.type == SDL_EVENT_QUIT) running = false;

		framebuffer.clearbuffer();

        // ---------------------------------------------------------
        // Triangle Drawing Tests (t0, t1, t2)
        // ---------------------------------------------------------

        // t0 - Red
        framebuffer.DrawTriangle(Vector2<int>(10, 70), Vector2<int>(50, 160), Vector2<int>(70, 80), 0xFFFF0000);

        // t1 - Green
        framebuffer.DrawTriangle(Vector2<int>(180, 50), Vector2<int>(150, 1), Vector2<int>(70, 180), 0xFF00FF00);

        // t2 - Blue
        framebuffer.DrawTriangle(Vector2<int>(180, 150), Vector2<int>(120, 160), Vector2<int>(130, 180), 0xFF0000FF);
        
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